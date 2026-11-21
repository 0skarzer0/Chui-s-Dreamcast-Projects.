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

#include "ML_Document.h"
#include "ML_leng_process.h"


/*
		Estas funciones pueden utilizarse en como constructores o 
	destructores de base llamandolas desde los constructores y destructores 
	de los nodos derivados...
*/

void ML_document_Constructor (p_com_obj obj)
{
	ML_p_document this;
	this=(ML_p_document) obj;

	this->ref=0;
	this->type=ML_DOCUMENT_1;

	strcpy(this->file_name,"");
	this->num_childs=0;
	this->childs=NULL;
}


void ML_document_Destructor (p_com_obj obj)
{
	int i;
	ML_p_document this;
	this=(ML_p_document) obj;

	for (i=0;i<this->num_childs;i++)
	{
		this->childs[i]->Release((p_com_obj)this->childs[i]);
	}
	
	if (this->childs) free(this->childs);

	free(this);
}


/*    Estas funciones pueden ser definidas de forma diferente en cada tipo de nodo.
	Aunque no es necesario ya que la liberacion de recursos se hace en el destructor.
*/

void ML_document_AddRef (p_com_obj this)
{
	this->ref++;
}

void ML_document_Release (p_com_obj this)
{
	this->ref--;
	if (this->ref<=0) this->Destructor(this);
}


/*   Esta funcion determina las interfaces disponibles*/

int ML_document_QueryInterface (p_com_obj this, int IID, void **p_IObj)
{
	switch(IID)
	{
	case IID_IMLDocument:
		*p_IObj = NULL;
		*p_IObj = malloc (sizeof(struct MLDocument));
		if (*p_IObj == NULL) 
		{
			/*Marcar error en sistema de definicion de errores*/
			return -1; /*No hay memoria disponible*/
		}
		
		ML_mldocument_init((IMLDocument)(*p_IObj),(ML_p_document)this);
		
		return 0; /*Todo bien*/

	default:
		/* Marcar el error en el sistema de definicion de errores*/

		return -2; /*Interfaz no reconocida*/	
	}

	return 0;
}

int ML_document_GetML (ML_p_document this, unsigned int *buf_size, char **out_buf)
{
	int i,j;
	unsigned int total_size, child_size;
	char **buf_list;

	*buf_size=0;
	*out_buf=NULL;
	
	if (this->num_childs==0)
		return 1;
	
	buf_list=(char **) malloc (this->num_childs*sizeof(char *));
	if (buf_list==NULL)
		return -1;

	j=0; 
	total_size=0;

	for (i=0;i<this->num_childs;i++)
	{
		if ( -1 == ( this->childs[i]->GetML( this->childs[i], &child_size, &(buf_list[j]) ) ) )
		{
			for (i=0; i<j;i++)
			{
				if (buf_list[i]) free (buf_list[i]);
			}
			if (buf_list) free (buf_list);
			*buf_size=child_size;
			return -1;
		}
		j++;
		total_size+=child_size-1;
	}

	/*Ya tenemos el tamaño que debera tener el buffer de salida, es total_size*/
	*out_buf=(char *) malloc (total_size+1);
	if (*out_buf==NULL)
	{
		for (i=0; i<j;i++)
		{
			if (buf_list[i]) free (buf_list[i]);
		}
		if (buf_list) free (buf_list);
		return -1;
	}
	
	strcpy(*out_buf,buf_list[0]);
	for (i=1;i<j;i++)
	{
		strcat(*out_buf,buf_list[i]);
	}
	
	*buf_size=total_size;

	/*Hay que liberar la memoria utilizada en el proceso*/
	if (buf_list) 
	{
		for (i=0; i<j;i++)
		{
			if (buf_list[i]) free (buf_list[i]);
		}
		free (buf_list);
	}

	return 0;
}

/*Inicializacion de un Documento version 1.0*/
void ML_document_init (ML_p_document documento)
{
	documento->Constructor=ML_document_Constructor;
	documento->Destructor=ML_document_Destructor;
	documento->AddRef=ML_document_AddRef;
	documento->Release=ML_document_Release;
	documento->QueryInterface=ML_document_QueryInterface;
	documento->GetML=ML_document_GetML;
	documento->Constructor((p_com_obj)documento);
}


/**********************************************************************
				Aqui comienza la interfaz IMLDocument
***********************************************************************/


void ML_mldocument_AddRef (IMLDocument this)
{
	this->p_this->AddRef((p_com_obj)this->p_this);
}

void ML_mldocument_Release (IMLDocument this)
{
	this->p_this->Release((p_com_obj)this->p_this);
	 
	free (this);
}

int ML_mldocument_QueryInterface (IMLDocument this, int IID, void **p_IObj)
{
	return this->p_this->QueryInterface((p_com_obj)this->p_this,IID,p_IObj);
}


int ML_mldocument_CreateProcessInstruction (IMLDocument this, int IID, IMLProcessInstruction *interfaz)
{
	ML_p_node node;
	
	switch (IID)
	{
	case IID_IMLProcessInstruction:
		node = malloc (sizeof(struct ML_node));
		if (node==NULL) return -1;
		ML_process_init(node);
		node->doc=this->p_this;
		*interfaz = malloc (sizeof(struct MLProcessInstruction));
		if (*interfaz==NULL) {free (node); return -1;}
		ML_mlprocess_init(*interfaz, node);
		return 0;

	default:
		return -2;
	}

}

int ML_mldocument_CreateElement (IMLDocument this, int IID, IMLElement *interfaz)
{
	ML_p_node node;
	
	switch (IID)
	{
	case IID_IMLElement:
		node = malloc (sizeof(struct ML_node));
		if (node == NULL) return -1;
		ML_element_init(node);
		node->doc=this->p_this;
		*interfaz = malloc (sizeof(struct MLElement));
		if (*interfaz == NULL) {free(node); return -1;}
		ML_mlelement_init(*interfaz, node);
		return 0;

	default:
		return -2;
	}

}

int ML_mldocument_CreateAttribute (IMLDocument this, int IID, IMLAttribute *interfaz)
{
	ML_p_node node;
	
	switch (IID)
	{
	case IID_IMLAttribute:
		node = malloc (sizeof(struct ML_node));
		if (node == NULL) return -1;
		ML_attribute_init(node);
		node->doc=this->p_this;
		*interfaz = malloc (sizeof(struct MLAttribute));
		if (*interfaz == NULL) {free(node); return -1;}
		ML_mlattribute_init(*interfaz, node);
		return 0;

	default:
		return -2;
	}
}

int ML_mldocument_CreateText (IMLDocument this, int IID, IMLText *interfaz)
{
	ML_p_node node;
	
	switch (IID)
	{
	case IID_IMLText:
		node = malloc (sizeof(struct ML_node));
		if (node == NULL) return -1;
		ML_text_init(node);
		node->doc=this->p_this;
		*interfaz = malloc (sizeof(struct MLText));
		if (*interfaz == NULL) {free(node); return -1;}
		ML_mltext_init(*interfaz, node);
		return 0;

	default:
		return -2;
	}
}

int ML_mldocument_CreateComment (IMLDocument this, int IID, IMLComment *interfaz)
{
	ML_p_node node;
	
	switch (IID)
	{
	case IID_IMLComment:
		node = malloc (sizeof(struct ML_node));
		if (node == NULL) return -1;
		ML_comment_init(node);
		node->doc=this->p_this;
		*interfaz = malloc (sizeof(struct MLComment));
		if (*interfaz == NULL) {free(node); return -1;}
		ML_mlcomment_init(*interfaz, node);
		return 0;

	default:
		return -2;
	}
}


int ML_mldocument_GetType (IMLDocument this)
{
	return this->p_this->type;
}

const char *ML_mldocument_GetFileName (IMLDocument this)
{
	return this->p_this->file_name;
}

int ML_mldocument_SetFileName (IMLDocument this, char * name)
{
	int i;

	/*Comprobamos si el nombre del fichero es valido*/
	if (strcmp(name,"")==0)
	{
		return -2; /*Nombre invalido*/
	}
	else	
	{
		for (i=0;i<strlen(name);i++)
		{
			if ((name[i]<45)||(name[i]>122))
				return -2; /*nombre invalido*/
		}
	}

	strncpy(this->p_this->file_name,name,256);
	return 0;
}

int ML_mldocument_GetNumChilds (IMLDocument this)
{
	return this->p_this->num_childs;
}

int ML_mldocument_GetSingleChild (IMLDocument this, char * name, IMLNode *out, int index)
{
	int i=index;
	*out=NULL;

	for (i;i<this->p_this->num_childs;i++)
	{
		if (strcmp(this->p_this->childs[i]->name,name)==0)
		{
			*out=(IMLNode) malloc (sizeof(struct MLNode));
			ML_mlnode_init(*out,this->p_this->childs[i]);
			return i;
		}
	}

	return -1;
}

int ML_mldocument_AppendChild (IMLDocument this, IMLNode child, IMLNode *ret)
{
	int i=0;
	ML_p_node tmp,*tmp1;

	if (child->p_this->type == ML_NODE_Attribute)
	{
		return -2;/*tipo de nodo no permitido para colgar de un documento*/
	}

	tmp = child->p_this->Copy(child->p_this);
	if (tmp==NULL)
	{
		*ret=NULL;
		return -1;
	}

	tmp1 = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->p_this->num_childs+1));

	if (tmp1==NULL) return -1; /*por si no hay memoria*/

	for (i;i<this->p_this->num_childs;i++)
	{
		tmp1[i]=this->p_this->childs[i];
	}

	tmp1[i]=tmp;
	tmp->AddRef((p_com_obj)tmp);
	tmp->doc=this->p_this;
	free(this->p_this->childs);
	this->p_this->childs=tmp1;
	this->p_this->num_childs++;

	*ret=(IMLNode) malloc(sizeof(struct MLNode));
	if (*ret)
	{
		ML_mlnode_init(*ret,tmp);
	}

	return i;
}

int ML_mldocument_DeleteChildByInterfaz (IMLDocument this, IMLNode child)
{
	int i,j,index;
	ML_p_node *tmp;

	for (index=0;index<this->p_this->num_childs;i++)
	{
		if (this->p_this->childs[index]==child->p_this)
			break;
	}

	if (index==this->p_this->num_childs)
		return 0;
	else
	{
		/*liberamos el enlace del objeto y si es el ultimo este se autodestruira*/
		this->p_this->childs[index]->Release((p_com_obj)this->p_this->childs[index]);

		/*Creamos la nueva lista de punteros de objeto*/
		tmp = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->p_this->num_childs-1));


		if (tmp==NULL) return -1; /*por si no hay memoria*/

		j=0;
		for (i=0;i<this->p_this->num_childs;i++)
		{
			if (i!=index)
				tmp[j++]=this->p_this->childs[i];
		}

		free (this->p_this->childs);/*borramos la lista vieja*/
		this->p_this->childs=tmp;/*ponemos la lista nueva*/
		this->p_this->num_childs--;
		return 1;
	}
}

int ML_mldocument_DeleteChildByIndex (IMLDocument this, int index)
{
	int i,j;
	ML_p_node *tmp;

	if ((index>=this->p_this->num_childs)||(index<0))
		return 0;
	else
	{
		/*liberamos el enlace del objeto y si es el ultimo este se autodestruira*/
		this->p_this->childs[index]->Release((p_com_obj)this->p_this->childs[index]);

		/*Creamos la nueva lista de punteros de objeto*/
		tmp = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->p_this->num_childs-1));


		if (tmp==NULL) return -1; /*por si no hay memoria*/

		j=0;
		for (i=0;i<this->p_this->num_childs;i++)
		{
			if (i!=index)
				tmp[j++]=this->p_this->childs[i];
		}

		free (this->p_this->childs);/*borramos la lista vieja*/
		this->p_this->childs=tmp;/*ponemos la lista nueva*/
		this->p_this->num_childs--;
		return 1;
	}
}



/*Esto tendra que soportar XPath o utilizar trucos tipo index*/
int ML_mldocument_GetSingleElement (IMLDocument this, char * name, IMLElement *out, int index)
{
	int i;
	i=index;

	for (i;i<this->p_this->num_childs;i++)
	{
		if ((this->p_this->childs[i]->type==ML_NODE_Element)&&(strcmp(this->p_this->childs[i]->name,name)==0))
		{
			*out=(IMLElement) malloc (sizeof(struct MLElement));
			if (*out==NULL) return -1;/*No hay memoria*/
			ML_mlelement_init(*out,this->p_this->childs[i]);
			return i;
		}
	}

	return -2;
}

int ML_mldocument_AppendElement (IMLDocument this, IMLElement in, IMLElement *ret)
{
	int i;
	ML_p_node tmp,*tmp1;


	tmp = in->p_this->Copy(in->p_this);
	if (tmp==NULL)
	{
		*ret=NULL;
		return -1;
	}

	tmp1 = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->p_this->num_childs+1));

	if (tmp1==NULL) return -1; /*por si no hay memoria*/

	for (i=0;i<this->p_this->num_childs;i++)
	{
		tmp1[i]=this->p_this->childs[i];
	}

	tmp1[i]=tmp;
	tmp->AddRef((p_com_obj)tmp);
	tmp->doc=this->p_this;
	free(this->p_this->childs);
	this->p_this->childs=tmp1;
	this->p_this->num_childs++;

	*ret=(IMLElement) malloc(sizeof(struct MLElement));
	if (*ret)
	{
		ML_mlelement_init(*ret,tmp);
	}

	return i;
}

/*Esto tendra que soportar tambien XPath para hacer mas facil todo. Para otra version*/
int ML_mldocument_DeleteElement (IMLDocument this, IMLElement in)
{
	int i,j,k;
	ML_p_node *tmp;

	for (i=0;i<this->p_this->num_childs;i++)
	{
		if ((this->p_this->childs[i]->type==ML_NODE_Element)&&(strcmp(this->p_this->childs[i]->name,in->p_this->name)==0))
		{
			/*liberamos el enlace del objeto y si es el ultimo este se autodestruira*/
			this->p_this->childs[i]->Release((p_com_obj)this->p_this->childs[i]);

			/*Creamos la nueva lista de punteros de objeto*/
			tmp = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->p_this->num_childs-1));


			if (tmp==NULL) return -1; /*por si no hay memoria*/

			j=0;
			for (k=0;k<this->p_this->num_childs;k++)
			{
				if (k!=i)
					tmp[j++]=this->p_this->childs[k];
			}

			free (this->p_this->childs);/*borramos la lista vieja*/
			this->p_this->childs=tmp;/*ponemos la lista nueva*/
			this->p_this->num_childs--;
			return 0;
		}
	}

	return -2;
}

/*Esto tendra que soportar XPath o utilizar trucos tipo index*/
int ML_mldocument_GetSingleProcessInstruction(IMLDocument this, char * name, IMLProcessInstruction *out, int index)
{
	int i;
	i=index;

	for (i;i<this->p_this->num_childs;i++)
	{
		if ((this->p_this->childs[i]->type==ML_NODE_ProcessInstruction)&&(strcmp(this->p_this->childs[i]->name,name)==0))
		{
			*out=(IMLProcessInstruction) malloc (sizeof(struct MLProcessInstruction));
			if (*out==NULL) return -1;/*No hay memoria*/
			ML_mlprocess_init(*out,this->p_this->childs[i]);
			return i;
		}
	}

	return -2;
}

int ML_mldocument_AppendProcessInstruction (IMLDocument this, IMLProcessInstruction in, IMLProcessInstruction *ret)
{
	int i;
	ML_p_node tmp,*tmp1;


	tmp = in->p_this->Copy(in->p_this);
	if (tmp==NULL)
	{
		*ret=NULL;
		return -1;
	}

	tmp1 = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->p_this->num_childs+1));

	if (tmp1==NULL) /*por si no hay memoria*/
	{
		tmp->Release((p_com_obj)tmp);
		*ret=NULL;
		return -1;
	}

	for (i=0;i<this->p_this->num_childs;i++)
	{
		tmp1[i]=this->p_this->childs[i];
	}

	tmp1[i]=tmp;
	tmp->AddRef((p_com_obj)tmp);
	tmp->doc=this->p_this;
	if (this->p_this->childs) free(this->p_this->childs);
	this->p_this->childs=tmp1;
	this->p_this->num_childs++;

	*ret=(IMLProcessInstruction) malloc(sizeof(struct MLProcessInstruction));
	if (*ret)
	{
		ML_mlprocess_init(*ret,tmp);
	}

	return i;
}


/*Esto tendra que soportar tambien XPath para hacer mas facil todo. Para otra version*/
int ML_mldocument_DeleteProcessInstruction (IMLDocument this, IMLProcessInstruction in)
{
	int i,j,k;
	ML_p_node *tmp;

	for (i=0;i<this->p_this->num_childs;i++)
	{
		if ((this->p_this->childs[i]->type==ML_NODE_ProcessInstruction)&&(strcmp(this->p_this->childs[i]->name,in->p_this->name)==0))
		{
			/*liberamos el enlace del objeto y si es el ultimo este se autodestruira*/
			this->p_this->childs[i]->Release((p_com_obj)this->p_this->childs[i]);

			/*Creamos la nueva lista de punteros de objeto*/
			tmp = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->p_this->num_childs-1));

			if (tmp==NULL) return -1; /*por si no hay memoria*/

			j=0;
			for (k=0;k<this->p_this->num_childs;k++)
			{
				if (k!=i)
					tmp[j++]=this->p_this->childs[k];
			}

			free (this->p_this->childs);/*borramos la lista vieja*/
			this->p_this->childs=tmp;/*ponemos la lista nueva*/
			this->p_this->num_childs--;
			return 0;
		}
	}

	return -2;
}

int ML_mldocument_AppendText (IMLDocument this, IMLText in, IMLText *ret)
{
	int i;
	ML_p_node tmp,*tmp1;


	tmp = in->p_this->Copy(in->p_this);
	if (tmp==NULL)
	{
		*ret=NULL;
		return -1;
	}

	tmp1 = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->p_this->num_childs+1));

	if (tmp1==NULL) /*por si no hay memoria*/
	{
		tmp->Release((p_com_obj)tmp);
		*ret=NULL;
		return -1;
	}

	for (i=0;i<this->p_this->num_childs;i++)
	{
		tmp1[i]=this->p_this->childs[i];
	}

	tmp1[i]=tmp;
	tmp->AddRef((p_com_obj)tmp);
	tmp->doc=this->p_this;
	if (this->p_this->childs) free(this->p_this->childs);
	this->p_this->childs=tmp1;
	this->p_this->num_childs++;

	*ret=(IMLText) malloc(sizeof(struct MLText));
	if (*ret)
	{
		ML_mltext_init(*ret,tmp);
	}

	return i;
}


int ML_mldocument_AppendComment (IMLDocument this, IMLComment in, IMLComment *ret)
{
	int i;
	ML_p_node tmp,*tmp1;


	tmp = in->p_this->Copy(in->p_this);
	if (tmp==NULL)
	{
		*ret=NULL;
		return -1;
	}

	tmp1 = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->p_this->num_childs+1));

	if (tmp1==NULL) /*por si no hay memoria*/
	{
		tmp->Release((p_com_obj)tmp);
		*ret=NULL;
		return -1;
	}

	for (i=0;i<this->p_this->num_childs;i++)
	{
		tmp1[i]=this->p_this->childs[i];
	}

	tmp1[i]=tmp;
	tmp->AddRef((p_com_obj)tmp);
	tmp->doc=this->p_this;
	if (this->p_this->childs) free(this->p_this->childs);
	this->p_this->childs=tmp1;
	this->p_this->num_childs++;

	*ret=(IMLComment) malloc(sizeof(struct MLText));
	if (*ret)
	{
		ML_mlcomment_init(*ret,tmp);
	}

	return i;
}


int ML_mldocument_GetML (IMLDocument this, unsigned int *buffer_size, char **out_buffer)
{
	return this->p_this->GetML(this->p_this,buffer_size,out_buffer);
}

int ML_mldocument_Save (IMLDocument this)
{
	int i;
	char *buf;
	unsigned int size;
	FILE *fichero;

	if (strcmp(this->p_this->file_name,"")==0)
	{
		return -2; /*Nombre invalido*/
	}	
	else	
	{
		for (i=0;i<strlen(this->p_this->file_name);i++)
		{
			if ((this->p_this->file_name[i]<45)||(this->p_this->file_name[i]>122))
				return -2; /*nombre invalido*/
		}
	}


	if (-1==this->p_this->GetML(this->p_this,&size,&buf))
	{
		return -1; /*No hay memoria para hacer la operacion*/
	}

	/*hacer control de errores*/
	fichero=fopen(this->p_this->file_name,"w");
	if (fichero==NULL) {free (buf); return -3;}
	if (fwrite(buf,sizeof(char),size,fichero)==0) {free (buf); return -4;}
	fclose(fichero);

	free(buf);

	return 0;	
}

int ML_mldocument_SaveFile (IMLDocument this, const char *name)
{
	int i;
	char *buf;
	unsigned int size;
	FILE *fichero;

	if (strcmp(name,"")==0)
	{
		return -2; /*Nombre invalido*/
	}
	else	
	{
		for (i=0;i<strlen(name);i++)
		{
			if ((name[i]<45)||(name[i]>122))
				return -2; /*nombre invalido*/
		}
	}

	if (-1==this->p_this->GetML(this->p_this,&size,&buf))
	{
		return -1; /*No hay memoria para hacer la operacion*/
	}

	fichero=fopen(name,"w");
	if (fichero==NULL) {free (buf); return -3;}
	if (fwrite((void *)buf,1,size,fichero)==0) {free (buf); return -4;}
	fclose(fichero);

	free(buf);

	return 0;	
}


int ML_mldocument_SetML (IMLDocument this, unsigned int buffer_size, char *in_buf)
{
	int i,res=0;
	FILE *fichero=NULL;
	IMLElement pe=NULL;
	IMLProcessInstruction pi=NULL;
	IMLComment pc=NULL;
	IMLText pt=NULL;

	/*si ya hay un documento cargado hay que descargarlo de memoria, esto no tiene vuelta atras*/
	for (i=0;i<this->p_this->num_childs;i++)
	{
		this->p_this->childs[i]->Release((p_com_obj)this->p_this->childs[i]);
	}
	if (this->p_this->childs) {free(this->p_this->childs); this->p_this->childs=NULL;}
	this->p_this->num_childs=0;


	/*Analizamos el buffer que tenemos en memoria*/
	i=0;
	while (i+4<buffer_size) 
	{
		switch (in_buf[i])
		{
		case '<':
			switch(in_buf[i+1])
			{
			case '?':
				if (ML_AnalizaPI(this,fichero,in_buf,&i,&buffer_size, &pi) == 0)
				{
					/*añadimos pi al documento*/
					if (ML_common_DocAddChild(this->p_this,pi->p_this)!=-1)
					{
						;;
					}
					if (pi) pi->Release(pi);
				}
				break;

			case '!':
				switch(in_buf[i+2])
				{
				case '-':
					if (in_buf[i+3]=='-')
					{
						if ( (ML_AnalizaC(this,fichero,in_buf,&i,&buffer_size, &pc)) == 0)
						{
/*							if (ML_common_DocAddChild(this->p_this,pc->p_this)!=-1)
							{
								;;
							}
*/							if (pc) pc->Release(pc);
						}/*ojo con los errores*/
					}
					break;

				case '[':
					if ( (ML_AnalizaD(this,fichero,in_buf,&i,&buffer_size, &pt)) == 0)
					{
						
						/*añadimos pt al documento*/
						if (ML_common_DocAddChild(this->p_this,pt->p_this)!=-1)
						{
							;;
						}
						if (pt) pt->Release(pt);
					}/*en caso de error hay que señalarlo*/
					break;
					
/*				case 'D':
					if ( (ML_AnalizaDTD(this,fichero,in_buf,&i,&buffer_size, &pdtd)) == 0)
					{
						;;
					}/*en caso de error hay que señalarlo*/
/*					break;
*/
				default:
					if ( (ML_AnalizaE(this,fichero,in_buf,&i,&buffer_size, &pe)) == 0)
					{
						if (ML_common_DocAddChild(this->p_this,pe->p_this)!=-1)
						{
							;;
						}
						if (pe) pe->Release(pe);
					}
					break;
					
				}
				break;

			default: 
				if ( ML_AnalizaE(this,fichero,in_buf,&i,&buffer_size, &pe) == 0)
				{
					/*añadimos pe al documento*/
					if (ML_common_DocAddChild(this->p_this,pe->p_this)!=-1)
					{
						;;
					}
					if (pe) pe->Release(pe);
				}
				break;
			}
			break;

		default:
			if ( (ML_AnalizaT(this,fichero,in_buf,&i,&buffer_size, &pt)) == 0)
			{
				
				/*añadimos pt al documento*/
				if (ML_common_DocAddChild(this->p_this,pt->p_this)!=-1)
				{
					;;
				}
				if (pt) pt->Release(pt);
			}
			break;
		}
	}

	return 0;	
}


/*Habra que hacer algunos cambios para cargar fichero grandes*/
int ML_mldocument_LoadFile (IMLDocument this, const char *name)
{
	int i, fin=0;
	char *buf;
	int size=0;
	FILE *fichero;

	/*Comprobamos si el nombre del fichero es valido*/
	if (strcmp(name,"")==0)
	{
		return -2; /*Nombre invalido*/
	}
	else	
	{
		for (i=0;i<strlen(name);i++)
		{
			if ((name[i]<45)||(name[i]>122))
				return -2; /*nombre invalido*/
		}
	}

	fichero=fopen(name,"r");
	if (fichero==NULL) return -3;

	/*busco el tamaño de fichero sin utilizar nada especifico del sistema*/
	fseek(fichero,0,SEEK_END);
	size=ftell(fichero);
	buf=(char *) malloc (size);
	if (buf==NULL) return -1;
	fseek(fichero,0,SEEK_SET); /*volvemos al inicio*/

	/*leemos el fichero en el buffer a medida*/
	if ((size=fread((void *)buf,1,size,fichero))==0) return -4;

	/*Analizamos el fichero que tenemos en memoria*/
	this->SetML(this,size, (char *)buf);

	if (buf) free(buf);

	fclose(fichero);

	this->SetFileName(this, name);

	return 0;	
}


/*Funcion de inicializacion de interfaz*/
void ML_mldocument_init (IMLDocument interfaz, ML_p_document objeto)
{
	interfaz->p_this = objeto;
	objeto->AddRef((p_com_obj)objeto);
	interfaz->AddRef = ML_mldocument_AddRef;
	interfaz->Release = ML_mldocument_Release;
	interfaz->QueryInterface = ML_mldocument_QueryInterface;

	interfaz->GetType = ML_mldocument_GetType;
	interfaz->GetFileName = ML_mldocument_GetFileName;
	interfaz->SetFileName = ML_mldocument_SetFileName;
	
	interfaz->CreateProcessInstruction = ML_mldocument_CreateProcessInstruction;
	interfaz->CreateElement = ML_mldocument_CreateElement;
	interfaz->CreateAttribute = ML_mldocument_CreateAttribute;
	interfaz->CreateText = ML_mldocument_CreateText;
	interfaz->CreateComment = ML_mldocument_CreateComment;

	interfaz->GetNumChilds = ML_mldocument_GetNumChilds;
	interfaz->GetSingleChild = ML_mldocument_GetSingleChild;
	interfaz->AppendChild = ML_mldocument_AppendChild;
	interfaz->DeleteChildByInterfaz = ML_mldocument_DeleteChildByInterfaz;
	interfaz->DeleteChildByIndex = ML_mldocument_DeleteChildByIndex;

	interfaz->AppendText = ML_mldocument_AppendText;

	interfaz->GetSingleElement = ML_mldocument_GetSingleElement;
	interfaz->AppendElement = ML_mldocument_AppendElement;
	interfaz->DeleteElement = ML_mldocument_DeleteElement;

	interfaz->GetSingleProcessInstruction = ML_mldocument_GetSingleProcessInstruction;
	interfaz->AppendProcessInstruction = ML_mldocument_AppendProcessInstruction;
	interfaz->DeleteProcessInstruction = ML_mldocument_DeleteProcessInstruction;

	interfaz->GetML = ML_mldocument_GetML;
	interfaz->SetML = ML_mldocument_SetML;

	interfaz->Save = ML_mldocument_Save;
	interfaz->SaveFile = ML_mldocument_SaveFile;
	interfaz->LoadFile = ML_mldocument_LoadFile;
}


/*Esto es un parche para evitar la utilizacion de una clase factoria, puede que en futuras versiones la utilice*/

int DocumentFromCOM (int IID, IMLDocument *interfaz)
{
	ML_p_document doc;
	
	switch (IID)
	{
	case IID_IMLDocument:
		doc = malloc (sizeof(struct ML_document));
		if (doc==NULL) return -1;
		ML_document_init(doc);
		*interfaz = malloc (sizeof(struct MLDocument));
		if (*interfaz==NULL) {free(doc); return -1;}
		ML_mldocument_init(*interfaz, doc);
		return 0;

	default:
		return -2;
	}

	return 0;
}


