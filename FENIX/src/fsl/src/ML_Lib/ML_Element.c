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

#include "ML_Element.h"
#include "ML_common_func.h"

/*************************************************************************

				Aqui trataremos los nodos elemento y su interfaz

**************************************************************************


/**************************************************************************
						Nodos de tipo elemento
***************************************************************************/

/*	El Constructor es lo unico que cambia por ahora.
	Tanto en el Destructor como la operacion Clone se utilizan la funciones
de la clase base nodo.
*/

void ML_element_Constructor (p_com_obj obj)
{
	ML_p_node this;
	this=(ML_p_node)obj;

	ML_node_Constructor(obj);
	this->type=ML_NODE_Element;
}

/*   En los elementos las unicas interfaces disponibles son IUnknown, 
IMLNode y IMLElement.
*/

int ML_element_QueryInterface (p_com_obj this, int IID, void **p_IObj)
{
	switch(IID)
	{
	case IID_IUnknown:
		*p_IObj = NULL;
		*p_IObj = malloc (sizeof(struct Unknown));
		if (*p_IObj == NULL) 
		{
			/*Marcar error en sistema de definicion de errores*/
			return -3; /*No hay memoria disponible*/
		}
		
		unknown_init((IUnknown)(*p_IObj),this);
		
		return 0; /*Todo bien*/

	case IID_IMLNode:
		*p_IObj = NULL;
		*p_IObj = malloc (sizeof(struct MLNode));
		if (*p_IObj == NULL) 
		{
			/*Marcar error en sistema de definicion de errores*/
			return -3; /*No hay memoria disponible*/
		}
		
		ML_mlnode_init((IMLNode)(*p_IObj),(ML_p_node)this);
		
		return 0; /*Todo bien*/

	case IID_IMLElement:
		*p_IObj = NULL;
		*p_IObj = malloc (sizeof(struct MLElement));
		if (*p_IObj == NULL) 
		{
			/*Marcar error en sistema de definicion de errores*/
			return -3; /*No hay memoria disponible*/
		}
		
		ML_mlelement_init((IMLElement)(*p_IObj),(ML_p_node)this);
		
		return 0; /*Todo bien*/

	default:
		/* Marcar el error en el sistema de definicion de errores*/

		return -2; /*Interfaz no reconocida*/	
	}

	return -1;
}

int ML_element_AppendChild (ML_p_node this, ML_p_node child, ML_p_node *ret)
{
	switch (child->type)
	{
	case ML_NODE_Element:
	case ML_NODE_Text:
	case ML_NODE_Comment:
		return ML_common_AppendChild(this,child,ret);

	case ML_NODE_Attribute: /*no puede haber dos atributos con el mismo nombre*/
		return ML_common_SetChild(this,child,ret);

	default:
		return -2;
	}
}

int ML_element_GetML (ML_p_node this, unsigned int *buf_size, char **out_buf)
{
	int i,j,num_attr;
	unsigned int total_size, child_size;
	char **buf_list;

	if (this->num_childs!=0)
	{
		buf_list=(char **) malloc (this->num_childs*sizeof(char *));
		if (buf_list==NULL)
		{
			return -1;
		}
	}
	else buf_list=NULL;

	j=0; /*este sera el indice para la el codigo de cada hijo*/
	total_size=0;

	/*Primero hago una pasada en busca de los atributos, ya que los hijos no llevan un orden obligatorio*/
	for (i=0;i<this->num_childs;i++)
	{
		if (this->childs[i]->type==ML_NODE_Attribute)
		{
			if ( -1 == ( this->childs[i]->GetML( this->childs[i], &child_size, &(buf_list[j]) ) ) )
			{
				for (i=0; i<j;i++)
				{
					if (buf_list[i]) free (buf_list[i]);
				}
				if (buf_list) free (buf_list);
				return -1;
			}
			j++;
			total_size+=child_size-1; /*para no tener en cuenta la terminacion \0 */
		}
	}

	num_attr=j;

	/*Añado los elementos que no son atributos, siguiendo el orden de aparicion*/
	for (i=0;i<this->num_childs;i++)
	{
		if (this->childs[i]->type!=ML_NODE_Attribute)
		{
			if ( -1 == ( this->childs[i]->GetML( this->childs[i], &child_size, &(buf_list[j]) ) ) )
			{
				for (i=0; i<j;i++)
				{
					if (buf_list[i]) free (buf_list[i]);
				}
				if (buf_list) free (buf_list);
				return -1;
			}
			j++;
			total_size+=child_size-1;
		}
	}

	/*Ya tenemos el tamaño que debera tener el buffer de salida*/
	/*el tamaño del (nombre x2) + 5 o 3 para las etiquetas y num_attr es por los espacios entre atributos*/
	if (num_attr==j) 
		total_size+=strlen(this->name)+num_attr+4; 
	else 
		total_size+=(strlen(this->name)*2)+num_attr+6;

	*out_buf=(char *) malloc (total_size);
	if (*out_buf==NULL)
	{
		for (i=0; i<j;i++)
		{
			if (buf_list[i]) free (buf_list[i]);
		}
		if (buf_list) free (buf_list);
		return -1;
	}
	
	strcpy(*out_buf,"<");
	strcat(*out_buf,this->name);
	for (i=0;i<num_attr;i++)
	{
		strcat(*out_buf," ");
		strcat(*out_buf,buf_list[i]);
	}
	if (num_attr==j)
		strcat(*out_buf,"/>");
	else
		strcat(*out_buf,">");
	
	for (i;i<j;i++)
	{
		strcat(*out_buf,buf_list[i]);
	}
	if (num_attr!=j)
	{
		strcat(*out_buf,"</");
		strcat(*out_buf,this->name);
		strcat(*out_buf,">");
	}
	
	*buf_size=total_size;

	/*Hay que liberar la memoria utilizada en el proceso*/
	for (i=0; i<j;i++)
	{
		if (buf_list[i]) free (buf_list[i]);
	}
	if (buf_list) free (buf_list);

	return 0;
}

/*Sencilla inicializacion de un nodo tipo elemento*/
void ML_element_init (ML_p_node nodo)
{
	nodo->Constructor=ML_element_Constructor;
	nodo->Destructor=ML_node_Destructor;
	nodo->AddRef=ML_node_AddRef;
	nodo->Release=ML_node_Release;
	nodo->QueryInterface=ML_element_QueryInterface;
	nodo->Copy=ML_node_Clone;
	nodo->AppendChild=ML_element_AppendChild;
	nodo->GetML=ML_element_GetML;
	nodo->Constructor((p_com_obj)nodo);
}

/******************************************************************************
	************************* IMLElement *******************************

	Aqui comienza la interfaz IMLElement para objetos del tipo nodo_elemento
*******************************************************************************/

void ML_mlelement_AddRef (IMLElement this)
{
	this->p_this->AddRef((p_com_obj)this->p_this);
}

void ML_mlelement_Release (IMLElement this)
{
	this->p_this->Release((p_com_obj)this->p_this);
	 
	free (this);
}

int ML_mlelement_QueryInterface (IMLElement this, int IID, void **p_IObj)
{
	return this->p_this->QueryInterface((p_com_obj)this->p_this,IID,p_IObj);
}

int ML_mlelement_GetType (IMLElement this)
{
	return this->p_this->type;
}

const char *ML_mlelement_GetName (IMLElement this)
{
	return this->p_this->name;
}

int ML_mlelement_SetName (IMLElement this, char * name)
{
	strncpy(this->p_this->name,name,256);
	return 0;
}

int ML_mlelement_GetNumChilds (IMLElement this)
{
	return this->p_this->num_childs;
}

int ML_mlelement_GetSingleChild (IMLElement this, char * name, IMLNode *out, int index)
{
	return ML_common_GetSingleChild (this->p_this, name, out, index);
}

int ML_mlelement_AppendChild (IMLElement this, IMLNode child, IMLNode *ret)
{
	ML_p_node tmp;
	int i;

	i=this->p_this->AppendChild (this->p_this, child->p_this, &tmp);
	if (i>=0)
	{
		*ret=(IMLNode) malloc (sizeof (struct MLNode));
		if (*ret!=NULL)	ML_mlnode_init(*ret,tmp);
	}
	return i;
}

int ML_mlelement_DeleteChildByInterfaz (IMLElement this, IMLNode child)
{
	return ML_common_DeleteChildByInterfaz (this->p_this, child->p_this);
}

int ML_mlelement_DeleteChildByIndex (IMLElement this, int index)
{
	return ML_common_DeleteChildByIndex (this->p_this, index);
}


int ML_mlelement_GetAttribute (IMLElement this, char * name, IMLAttribute *out)
{
	int i;

	for (i=0;i<this->p_this->num_childs;i++)
	{
		if ((this->p_this->childs[i]->type==ML_NODE_Attribute)&&(strcmp(this->p_this->childs[i]->name,name)==0))
		{
			*out=(IMLAttribute) malloc (sizeof(struct MLAttribute));
			if (*out==NULL) return -1;/*No hay memoria*/
			ML_mlattribute_init(*out,this->p_this->childs[i]);
			return i;
		}
	}

	return -2;
}

int ML_mlelement_SetAttribute (IMLElement this, IMLAttribute in)
{
	ML_p_node tmp;
	
	return ML_common_SetChild(this->p_this,in->p_this,&tmp);
}

int ML_mlelement_DeleteAttribute (IMLElement this, char *name)
{
	return ML_common_DeleteChildByName(this->p_this,name,ML_NODE_Attribute);
}


/*Esto tendra que soportar XPath o utilizar trucos tipo index*/
int ML_mlelement_GetSingleElement (IMLElement this, char * name, IMLElement *out, int index)
{
	int i=index;

	for (;i<this->p_this->num_childs;i++)
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

int ML_mlelement_AppendElement (IMLElement this, IMLElement in, IMLElement *ret)
{
	ML_p_node tmp;
	int i;

	i=this->p_this->AppendChild (this->p_this, in->p_this, &tmp);
	if (i>=0)
	{
		*ret=(IMLElement) malloc (sizeof (struct MLElement));
		if (*ret!=NULL)	ML_mlelement_init(*ret,tmp);
	}
	return i;
}

/*Esto tendra que soportar tambien XPath para hacer mas facil todo. Para otra version*/
int ML_mlelement_DeleteElement (IMLElement this, IMLElement in)
{
	return ML_common_DeleteChildByInterfaz (this->p_this, in->p_this);
}

int ML_mlelement_AppendText (IMLElement this, IMLText in, IMLText *ret)
{
	ML_p_node tmp;
	int i;

	i=this->p_this->AppendChild (this->p_this, in->p_this, &tmp);
	if (i>=0)
	{
		*ret=(IMLText) malloc (sizeof (struct MLText));
		if (*ret!=NULL)	ML_mltext_init(*ret,tmp);
	}
	return i;
}

int ML_mlelement_AppendComment (IMLElement this, IMLComment in, IMLComment *ret)
{
	ML_p_node tmp;
	int i;

	i=this->p_this->AppendChild (this->p_this, in->p_this, &tmp);
	if (i>=0)
	{
		*ret=(IMLComment) malloc (sizeof (struct MLComment));
		if (*ret!=NULL)	ML_mlcomment_init(*ret,tmp);
	}
	return i;
}

int ML_mlelement_GetML (IMLElement this, unsigned int *buffer_size, char **out_buffer)
{
	return this->p_this->GetML(this->p_this,buffer_size,out_buffer);
}


/*Funcion de inicializacion de interfaz*/
void ML_mlelement_init (IMLElement interfaz, ML_p_node objeto)
{
	interfaz->p_this = objeto;
	objeto->AddRef((p_com_obj)objeto);
	interfaz->AddRef = ML_mlelement_AddRef;
	interfaz->Release = ML_mlelement_Release;
	interfaz->QueryInterface = ML_mlelement_QueryInterface;
	interfaz->GetType = ML_mlelement_GetType;
	interfaz->GetName = ML_mlelement_GetName;
	interfaz->SetName = ML_mlelement_SetName;
	interfaz->GetNumChilds = ML_mlelement_GetNumChilds;
	interfaz->GetSingleChild = ML_mlelement_GetSingleChild;
	interfaz->AppendChild = ML_mlelement_AppendChild;
	interfaz->DeleteChildByInterfaz = ML_mlelement_DeleteChildByInterfaz;
	interfaz->DeleteChildByIndex = ML_mlelement_DeleteChildByIndex;
	interfaz->GetAttribute = ML_mlelement_GetAttribute;
	interfaz->SetAttribute = ML_mlelement_SetAttribute;
	interfaz->DeleteAttribute = ML_mlelement_DeleteAttribute;
	interfaz->GetSingleElement = ML_mlelement_GetSingleElement;
	interfaz->AppendElement = ML_mlelement_AppendElement;
	interfaz->DeleteElement = ML_mlelement_DeleteElement;
	interfaz->AppendText = ML_mlelement_AppendText;
	interfaz->AppendComment = ML_mlelement_AppendComment;
	interfaz->GetML = ML_mlelement_GetML;
}



