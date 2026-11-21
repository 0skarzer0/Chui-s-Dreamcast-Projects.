/*---------------------------------------------------------------------------

	                  GNU Lesser General Public License

------------------------------------------------------------------------------
    Markup Languaje Library ML_Lib version 0.1
    Copyright (C) 2004  Diego Blazquez Garcia, alias (Mortimor)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

------------------------------------------------------------------------------*/

#include "ML_leng_process.h"

/*
   Descripcion: comprueba si un caracter es o no reservado. 
   Datos de entrada: c.
*/

int ML_is_reserv (char c)
{
	/*Puede que estos varien en futuras versiones*/
	if ( ((c<0x30)&&(c!=0x2D)) || ((c>0x3A)&&(c<0x41)) || ((c>0x5a)&&(c<0x61)&&(c!=0x5f)) || ((c>0x7a)&&(c<0x80)) )
		return 1;
	else 
		return 0;
}


/*
   Descripcion: salta todos los espacios y saltos de linea. 
   Datos de entrada: buf, size e index.
   Datos de salida: old_index
*/

int ML_salta_espacio (char *buf, int size, int *index, int *old_index)
{
	*old_index=*index;

	 while ((*index<size)&&((buf[*index]==' ')||(buf[*index]=='\n'))) {(*index)++;}

	if (*index>=size)
		return -1;
	else
		return 0;
}

/*
   Descripcion: salta todos los caracteres hasta encontrar un signo de menor. 
   Datos de entrada: buf, size e index.
   Datos de salida: old_index
*/

int ML_busca_menor (char *buf, int size, int *index, int *old_index)
{
	*old_index=*index;

	while (*index<size)
	{
		if (buf[*index]=='<') break;
		(*index)++;
	}

	if (*index==size)
		return -1;
	else
		return 0;
}

/*
   Descripcion: salta todos los caracteres hasta encontrar la cadena especificada. 
   Datos de entrada: buf, size, cad e index.
   Datos de salida: old_index
*/

int ML_busca_cadena (char *buf, int size, char *cad, int *index, int *old_index)
{
	int i,tam;
	
	*old_index=*index;
	tam=strlen(cad);
	i=0;
	
	while (*index<size)
	{
		if (buf[*index]==cad[i])
		{	
			i++;
			if (i==tam) break;
		}
		else i=0;
		(*index)++;
	}

	if (*index==size)
		return -1;
	else
		return 0;
}

/*
   Descripcion: salta todos los caracteres hasta encontrar un signo de mayor. 
   Datos de entrada: buf, size, index y nom_busq
   Datos de salida: old_index
*/

int ML_busca_etiqueta_fin (char *buf, int size, int *index, int *old_index, char *nom_busq)
{
	int idx2,res=0;

	*old_index=*index;

	if (buf[*index]=='<')
	{
		if ((*index+1<size)&&(buf[*index+1]=='/'))
		{
			*index+=2;
		}
		else return -1;
	}
	else return -3;

	if (ML_salta_espacio(buf, size, index, &idx2)==-1) return -1;

	switch (ML_coger_nombre(buf, size, index, &idx2))
	{
	case -1:
		return -1;
	case -2:
	case 0:
		if (buf[*index]!='>') return -2;
		if (strncmp(&buf[idx2],nom_busq,*index-idx2)==0) res=1;
		break;
	}
		
	if (ML_salta_espacio(buf, size, index, &idx2)==-1) return -1;

	if (buf[*index]=='>')
		if (res) {(*index)++; return 0;} else {(*index)++; return 1;}
	else
		if (res) return 2; else return -4;

}

/*
   Descripcion: recoge todos los caracteres validos para formar un nombre (* - reservados - ' ' - '\n'). 
   Datos de entrada: buf, size e index.
   Datos de salida: old_index
*/

int ML_coger_nombre (char *buf, int size, int *index, int *old_index)
{
	*old_index=*index;

	 while (*index<size)
	{
		if ((buf[*index]==' ') || (buf[*index]=='\n')) return 0;
		if (ML_is_reserv(buf[*index]) == 1) return -2;

		(*index)++;
	}

	return -1;
}


/*
   Descripcion: recoge los caracteres que determinan el valor de un atributo. 
   Datos de entrada: buf, size e index.
   Datos de salida: old_index
*/

int ML_coger_atr_valor (char *buf, int size, int *index, int *old_index)
{
	if (*index>=size) return -1;

	if (buf[*index]!='"') return -3; else (*index)++; 

	*old_index=*index;

	 while (*index<size) 
	{
		if (buf[*index]=='"')
		{
			if ( (buf[*index-1]!='\\') || ( (buf[*index-1]=='\\') && ((*index - *old_index)>2) && (buf[*index-2]!='\\') ) )
			{
				(*index)++; 
				return 0;
			}
		}
		/*if ( (ML_is_reserv(buf[*index]) != 0) ) return -2;*/

		(*index)++;
	}

	return -1;
}


int ML_AnalizaT(IMLDocument doc, FILE *fichero, char *buf, int *i, int *size, IMLText *tmp)
{
	int old_index;
	ML_p_node tmp_node;

	ML_busca_menor ( buf, *size, i, &old_index);

	if (*i==old_index) return 1;

	if (doc->CreateText(doc,IID_IMLText,tmp)!=0) return -2;

	tmp_node=(*tmp)->p_this; /*para ahorrar redirecciones*/
	tmp_node->data=malloc(*i-old_index);
	if (tmp_node->data==NULL) {(*tmp)->Release(*tmp); return -1;}
	tmp_node->size=(*i-old_index);
	memcpy(tmp_node->data,&buf[old_index],*i-old_index);

	return 0;
}

int ML_AnalizaD(IMLDocument doc, FILE *fichero, char *buf, int *i, int *size, IMLText *tmp)
{
	int old_index;
	ML_p_node tmp_node;

	/*comprobamos el inicio de Comentario*/
	if ((*size)-(*i)<10) return -3;
	if (strncmp("<![CDATA[",&buf[*i],9)) return -3; /*habria que indicar el error*/

	*i+=9;

	old_index=*i;

	while (*i<*size)
	{
		if ((buf[*i]=='>')&&(buf[*i-1]==']')&&(buf[*i-2]==']')) {(*i)++; break;}
		(*i)++;
	}

	if (*i==*size) return -3;

	if (*i==old_index) return 0;

	if (doc->CreateText(doc,IID_IMLText,tmp)!=0) return -2;

	tmp_node=(*tmp)->p_this; /*para ahorrar redirecciones*/
	tmp_node->data=malloc(*i-old_index);
	if (tmp_node->data==NULL) {(*tmp)->Release(*tmp); return -1;}
	tmp_node->size=(*i-old_index);
	memcpy(tmp_node->data,&buf[old_index],*i-old_index);

	return 1;
}


int ML_AnalizaC(IMLDocument doc, FILE *fichero, char *buf, int *i, int *size, IMLComment *tmp)
{
	int old_index;
	ML_p_node tmp_node;

	/*comprobamos el inicio de Comentario*/
	if (!((buf[*i]=='<')&&(buf[*i+1]=='!')&&(buf[*i+2]=='-')&&(buf[*i+3]=='-'))) return -3; /*habria que indicar el error*/

	*i+=3;

	old_index=*i;

	while (*i<*size)
	{
		if ((buf[*i]=='>')&&(buf[*i-1]=='-')&&(buf[*i-2]=='-')) {(*i)++; break;}
		(*i)++;
	}

	if (*i==*size) return -3;

	if (*i==old_index) return 0;

	if (doc->CreateComment(doc,IID_IMLComment,tmp)!=0) return -2;

	tmp_node=(*tmp)->p_this; /*para ahorrar redirecciones*/
	tmp_node->data=malloc(*i-old_index);
	if (tmp_node->data==NULL) {(*tmp)->Release(*tmp); return -1;}
	tmp_node->size=(*i-old_index);
	memcpy(tmp_node->data,&buf[old_index],*i-old_index);

	return 1;
}

int ML_AnalizaPI(IMLDocument doc, FILE *fichero, char *buf, int *i, int *size, IMLProcessInstruction *tmp)
{
	IMLAttribute atr=NULL;
	int old_index;
	ML_p_node tmp_node;

	/*comprobamos el inicio de PI*/
	if (!((buf[*i]=='<')&&(buf[*i+1]=='?'))) return -3; /*habria que indicar el error*/

	*i+=2;
	
	if (ML_salta_espacio(buf, *size, i, &old_index)==-1) /*Se acabo el buffer. Aqui se podria recargar, pero por ahora no*/
	{
		return -2;
	}

	switch (ML_coger_nombre(buf, *size, i, &old_index))
	{
	case -1:
		return -2;

	case 0:
		break;

	case -2:
		if ((buf[*i]=='?')&&(buf[*i+1]=='>'))
			break;
		else
			return -4;
	}

	if (doc->CreateProcessInstruction(doc,IID_IMLProcessInstruction,tmp)!=0) return -5;

	tmp_node=(*tmp)->p_this; /*para ahorrar redirecciones*/

	if (*i-old_index>255) {(*tmp)->Release(*tmp); return -1;}
	else 
	{
		memcpy(tmp_node->name,&buf[old_index],*i-old_index);
		tmp_node->name[*i-old_index]='\0';
	}

	/*ahora hay que buscar atributos o el final*/
	while (*i<*size) 
	{
		if (ML_salta_espacio(buf, *size, i, &old_index)==-1)
		{
			return 1;
		}
	
		if (buf[*i]=='?')
		{
			if (*i+1>=*size) {if (atr) atr->Release(atr); return 0; }
			else if (buf[*i+1]=='>') {if (atr) atr->Release(atr); *i+=2; return 0; }
			else {if (atr) atr->Release(atr); return 0; }
		}
		else if (ML_is_reserv(buf[*i])) 
			{/*Si encuentra un caracter reservado aqui lo considera un error, destruye la PI y devuelve el control*/
				if (atr) atr->Release(atr); 
				if (*tmp) (*tmp)->Release(*tmp); 
				return -6;
			}

		switch (ML_coger_nombre(buf, *size, i, &old_index))
		{
		case -1:
			return 0;
	
		case 0:
		case -2:
			if (doc->CreateAttribute(doc,IID_IMLAttribute,&atr)!=0)
			{
				return 0;
			}
			
			tmp_node=atr->p_this; /*para ahorrar redirecciones*/

			if (*i-old_index>255) 
			{
				memcpy(tmp_node->name,&buf[old_index],255);
				tmp_node->name[255]='\0';
			}
			else 
			{
				memcpy(tmp_node->name,&buf[old_index],*i-old_index);
				tmp_node->name[*i-old_index]='\0';
			}
			break;
		}

		if (ML_salta_espacio(buf, *size, i, &old_index)==-1)
		{
			if (atr) atr->Release(atr);
			return 0;
		}

		if (buf[*i] == '=')
		{
			
			(*i)++;
	
			if (ML_salta_espacio(buf, *size, i, &old_index)==-1)
			{
				if (atr) atr->Release(atr);
				return 0;
			}
	
			switch (ML_coger_atr_valor(buf, *size, i, &old_index))
			{
			case -1:
				return 0;
		
			case 0:
				tmp_node->data=malloc(*i-old_index-1);
				tmp_node->size=(*i-old_index-1);
				memcpy(tmp_node->data,&buf[old_index],*i-old_index-1);
				break;
		
			case -3: /*Aqui deberia dar error, pero por ahora dejaremos que sea flexible en este aspecto*/
				break;
			}
		}

		/*Añadimos el atributo si lo hemos encontramos*/
		if ((*tmp)->SetAttribute(*tmp,atr)==-1) 
		{
			if (atr) atr->Release(atr);
			return 0;
		}

		if (atr) atr->Release(atr);
		atr=NULL;
	}
	
	/*Se acabo el buffer, mientras no lo recargemos aqui devolveremos el control.*/
	if (atr) atr->Release(atr);
	return 0;	
}

int ML_AnalizaE(IMLDocument doc, FILE *fichero, char *buf, int *i, int *size, IMLElement *tmp)
{
	IMLElement pe=NULL;
	IMLText pt=NULL;
	IMLComment pc=NULL;
	IMLAttribute atr=NULL;
	int old_index;
	ML_p_node tmp_node,tmp_node2;

	/*comprobamos el inicio del Ele*/
	if (buf[*i]!='<') return -3; /*habria que indicar el error*/

	(*i)++;
	
	if (ML_salta_espacio(buf, *size, i, &old_index)==-1) /*Se acabo el buffer. Aqui se podria recargar, pero por ahora no*/
	{
		return -2;
	}

	switch (ML_coger_nombre(buf, *size, i, &old_index))
	{
	case -1:
		return -2;

	case 0:
		if (*i-old_index<1) return -2;
		break;

	case -2:
		if ( ((buf[*i]=='/')&&(buf[*i+1]=='>')) || (buf[*i]=='>') )
			break;
		else
			return -4;
	}

	if (doc->CreateElement(doc,IID_IMLElement,tmp)!=0) return -5;

	tmp_node=(*tmp)->p_this; /*para ahorrar redirecciones*/

	if (*i-old_index>255) {(*tmp)->Release(*tmp); return -1;}
	else 
	{
		memcpy(tmp_node->name,&buf[old_index],*i-old_index);
		tmp_node->name[*i-old_index]='\0';
	}

	/*ahora hay que buscar atributos o el final de la etiqueta*/
	while (*i<*size) 
	{
		if (ML_salta_espacio(buf, *size, i, &old_index)==-1)
		{
			return 0;
		}
	
		if (buf[*i]=='/') /*posibles finales de elemento, para elementos vacios*/
		{
			if (*i+1>=*size) {if (atr) atr->Release(atr); return 0; }
			else if (buf[*i+1]=='>') {if (atr) atr->Release(atr); (*i)+=2; return 0; }
			else {if (atr) atr->Release(atr); (*i)++; return 0; } /*por ahora dejare que pase*/
		}

		if (buf[*i]!='>') /*se admite todo menos fin de buffer y falta de memoria*/
		{
			if (ML_is_reserv(buf[*i])) 
			{/*Si encuentra un caracter reservado aqui lo considera un error, destruye el Ele y devuelve el control*/
				if (atr) atr->Release(atr); 
				if (tmp) (*tmp)->Release(*tmp); 
				return -6;
			}
	
			switch (ML_coger_nombre(buf, *size, i, &old_index))
			{
			case -1:
				return 0;
		
			case 0:
			case -2:
				if (doc->CreateAttribute(doc,IID_IMLAttribute,&atr)!=0)
				{
					return 0;
				}
	
				tmp_node2=atr->p_this; /*para ahorrar redirecciones*/
				if (*i-old_index>255) 
				{
					memcpy(tmp_node2->name,&buf[old_index],255);
					tmp_node2->name[255]='\0';
				}
				else 
				{
					memcpy(tmp_node2->name,&buf[old_index],*i-old_index);
					tmp_node2->name[*i-old_index]='\0';
				}
				break;
			}
	
			if (ML_salta_espacio(buf, *size, i, &old_index)==-1)
			{
				if (atr) atr->Release(atr);
				return 0;
			}
	
			if (buf[*i] == '=')
			{
				
				(*i)++;
		
				if (ML_salta_espacio(buf, *size, i, &old_index)==-1)
				{
					if (atr) atr->Release(atr);
					return 0;
				}
		
				switch (ML_coger_atr_valor(buf, *size, i, &old_index))
				{
				case -1:
					return 0;
			
				case 0:
					tmp_node2->data=malloc(*i-old_index-1);
					tmp_node2->size=(*i-old_index-1);
					memcpy(tmp_node2->data,&buf[old_index],*i-old_index-1);
					break;
			
				case -3: /*Aqui deberia dar error, pero por ahora dejaremos que sea flexible en este aspecto*/
					break;
				}
			}
	
			/*Añadimos el atributo si lo hemos encontramos*/
			if ((*tmp)->SetAttribute(*tmp,atr)==-1) 
			{
				if (atr) atr->Release(atr);
				return 0;
			}
	
			if (atr) atr->Release(atr);
			atr=NULL;

		}
		else 
		{
			(*i)++;
			break; /*si encuentra el final de la etiqueta '>' salimos de este bucle*/
		}
	}
	
	while (*i<*size-1)
	{
		switch(buf[*i])
		{
		case '<':
			switch (buf[*i+1])
			{/*etiqueta de fin de elemento*/
			case '/':
				switch (ML_busca_etiqueta_fin(buf, *size, i, &old_index, tmp_node->name))
				{
				case 0:/*hay una etiqueta de fin y pertenece a nuestro elemento*/
					return 0;
				case 2:/*no es un fin de etiqueta valido pero aparece el nombre que buscamos*/
					return 0;
				
				case 1: /*hay una etiqueta de fin, pero no es de nuestro elemento*/
					break;
				case -4: /*no es un fin de etiqueta valido y no aparece el nombre que buscamos*/
					break;
				}
				break;

			case '!':
				switch (buf[*i+2])
				{
				case '-':
					if (buf[*i+2]=='-')
					{
						if ( (ML_AnalizaC(doc,fichero,buf,i,size, &pc)) == 0)
						{
							/*añadimos pc al elemento tmp, utilizo add para ahorrar mem y cpu*/
							if (ML_common_AddChild(tmp_node,pc->p_this)!=-1)
							{
								;; /*no hacemos nada*/
							}
							if (pc) {pc->Release(pc); pc=NULL;}
						}
					}
					break;

				case '[':
					if ( (ML_AnalizaD(doc,fichero,buf,i,size, &pt)) == 0)
					{
						/*añadimos pt al elemento tmp*/
						if (ML_common_AddChild(tmp_node,pt->p_this)!=-1)
						{
							;;
						}
						if (pt) {pt->Release(pt); pt=NULL;}
					}
					break;

				default:/*error, hay que gestionar los errores*/
					break;
				}
				break;

			default:
				if ( (ML_AnalizaE(doc,fichero,buf,i,size, &pe)) == 0)
				{
					/*añadimos pe al elemento tmp*/
					if (ML_common_AddChild(tmp_node,pe->p_this)!=-1)
					{
						;;
					}
					if (pe) {pe->Release(pe); pe=NULL;}
				}
				break;
			}
			break;

		default:
			if ( (ML_AnalizaT(doc,fichero,buf,i,size, &pt)) == 0)
			{
				/*añadimos pt al elemento*/
				if (ML_common_AddChild(tmp_node,pt->p_this)!=-1)
				{
					;;
				}
				if (pt) {pt->Release(pt); pt=NULL;}
			}
			break;
		}
	}

	/*Se acabo el buffer, mientras no lo recargemos aqui devolveremos el control.*/
	if (atr) atr->Release(atr);
	if (pt) pt->Release(pt);
	if (pe) pe->Release(pe);
	return 0;	
}

