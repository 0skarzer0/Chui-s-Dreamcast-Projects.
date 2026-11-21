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


#include "ML_Attribute.h"
#include "ML_common_func.h"

/*************************************************************************

				Aqui trataremos los nodos Atributo y su interfaz

**************************************************************************

**************************************************************************

						Nodos de tipo Atributo

**************************************************************************/



/*	El Constructor es lo unico que cambia por ahora.
	Tanto en el Destructor como la operacion Clone se utilizan la funciones
de la clase base nodo.
*/

void ML_attribute_Constructor (p_com_obj this)
{
	ML_node_Constructor((p_com_obj)this);
	this->type=ML_NODE_Attribute;
}

/*   En los Atributos las unicas interfaces disponibles son IUnknown, 
IMLNode y IMLAttribute.
*/

int ML_attribute_QueryInterface (p_com_obj this, int IID, void **p_IObj)
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

	case IID_IMLAttribute:
		*p_IObj = NULL;
		*p_IObj = malloc (sizeof(struct MLAttribute));
		if (*p_IObj == NULL) 
		{
			/*Marcar error en sistema de definicion de errores*/
			return -3; /*No hay memoria disponible*/
		}
		
		ML_mlattribute_init((IMLAttribute)(*p_IObj),(ML_p_node)this);
		
		return 0; /*Todo bien*/

	default:
		/* Marcar el error en el sistema de definicion de errores*/

		return -2; /*Interfaz no reconocida*/	
	}

	return -1;
}

int ML_attribute_AppendChild (ML_p_node this, ML_p_node child, ML_p_node *ret)
{
	if (child->type!=ML_NODE_Text) return -2;
	return ML_common_AppendChild(this,child,ret);
}

int ML_attribute_GetML (ML_p_node this, unsigned int *buf_size, char **out_buf)
{
	int i,num_txt;
	unsigned int total_size, child_size;
	char **buf_list;

	total_size=0;

	if (this->size==0)  /*tiene preferencia el campo data sobre los nodos hijos*/
	{
		if (this->num_childs!=0)
		{
			buf_list=(char **) malloc (this->num_childs*sizeof(char *));
			if (buf_list==NULL)
			{
				return -1;
			}
		}
		else buf_list=NULL;
	
		num_txt=0; /*este sera el indice para la el codigo de cada hijo*/
	
		/*Recogemos los trozos de texto, no puede haber otra cosa*/
		for (i=0;i<this->num_childs;i++)
		{
			if (this->childs[i]->type==ML_NODE_Text)
			{
				if ( -1 == ( this->childs[i]->GetML( this->childs[i], &child_size, &(buf_list[num_txt]) ) ) )
				{
					for (i=0; i<num_txt;i++)
					{
						free (buf_list[i]);
					}
					if (buf_list) free (buf_list);
					return -1;
				}
				num_txt++;
				total_size+=child_size;
			}
		}
	
		/*Ya tenemos el tamaño que debera tener el buffer de salida*/
		/*el tamaño del nombre + 3 para la etiqueta y num_txt es por los espacios entre atributos*/
		total_size+=strlen(this->name)+3+num_txt;	
	
		*out_buf=(char *) malloc (total_size);
		if (*out_buf==NULL)
		{
			for (i=0; i<num_txt;i++)
			{
				free (buf_list[i]);
			}
			if (buf_list) free (buf_list);
			return -1;
		}
		
		strcpy(*out_buf,this->name);
		strcat(*out_buf,"=\"");
		for (i=0;i<num_txt;i++)
		{
			strcat(*out_buf,buf_list[i]);
		}
		strcat(*out_buf,"\"");
		
		*buf_size=total_size;

		/*Hay que liberar la memoria utilizada en el proceso*/
		for (i=0; i<num_txt;i++)
		{
			free (buf_list[i]);
		}
		if (buf_list) free (buf_list);
	}
	else
	{
		/*el tamaño del nombre + 3 para la etiqueta*/
		total_size+=this->size;
		total_size+=strlen(this->name)+4;	
		*out_buf=(char *) malloc (total_size);
		if (*out_buf==NULL) return -1;
		*buf_size=total_size;

		strcpy(*out_buf,this->name);
		strcat(*out_buf,"=\"");
		memcpy(*out_buf+(strlen(*out_buf)*sizeof(char)),this->data, this->size); /*no es una cadena lo que copiamos*/
		(*out_buf)[total_size-2]='"';
		(*out_buf)[total_size-1]='\0';
	}

	return 0;
}

/*Sencilla inicializacion de un nodo tipo Attributeo*/
void ML_attribute_init (ML_p_node nodo)
{
	nodo->Constructor=ML_attribute_Constructor;
	nodo->Destructor=ML_node_Destructor;
	nodo->AddRef=ML_node_AddRef;
	nodo->Release=ML_node_Release;
	nodo->QueryInterface=ML_attribute_QueryInterface;
	nodo->Copy=ML_node_Clone;
	nodo->GetML=ML_attribute_GetML;
	nodo->AppendChild=ML_attribute_AppendChild;
	nodo->Constructor((p_com_obj)nodo);
}

/******************************************************************************
	************************* IMLAttribute *******************************

	Aqui comienza la interfaz IMLAttribute para objetos del tipo nodo_Attributeo
*******************************************************************************/

void ML_mlattribute_AddRef (IMLAttribute this)
{
	this->p_this->AddRef((p_com_obj)this->p_this);
}

void ML_mlattribute_Release (IMLAttribute this)
{
	this->p_this->Release((p_com_obj)this->p_this);
	 
	free (this);
}

int ML_mlattribute_QueryInterface (IMLAttribute this, int IID, void **p_IObj)
{
	return this->p_this->QueryInterface((p_com_obj)this->p_this,IID,p_IObj);
}

int ML_mlattribute_GetType (IMLAttribute this)
{
	return this->p_this->type;
}

const char *ML_mlattribute_GetName (IMLAttribute this)
{
	return this->p_this->name;
}

int ML_mlattribute_SetName (IMLAttribute this, char * name)
{
	strncpy(this->p_this->name,name,256);
	return 0;
}

int ML_mlattribute_GetNumChilds (IMLAttribute this)
{
	return this->p_this->num_childs;
}

int ML_mlattribute_GetSingleChild (IMLAttribute this, char * name, IMLNode *out, int index)
{
	return ML_common_GetSingleChild (this->p_this, name, out, index);
}

int ML_mlattribute_AppendChild (IMLAttribute this, IMLNode child, IMLNode *ret)
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

int ML_mlattribute_DeleteChildByInterfaz (IMLAttribute this, IMLNode child)
{
	return ML_common_DeleteChildByInterfaz (this->p_this, child->p_this);
}

int ML_mlattribute_DeleteChildByIndex (IMLAttribute this, int index)
{
	return ML_common_DeleteChildByIndex (this->p_this, index);
}


int ML_mlattribute_GetDataSize (IMLAttribute this)
{
	return this->p_this->size;
}

int ML_mlattribute_GetData (IMLAttribute this, int *buffer_size, void **out_buffer)
{
	return ML_common_GetData (this->p_this, buffer_size, out_buffer);
}

int ML_mlattribute_SetData (IMLAttribute this, int buffer_size, void *in_buffer)
{
	return ML_common_SetData (this->p_this, buffer_size, in_buffer);
}

int ML_mlattribute_DeleteData (IMLAttribute this)
{
	return ML_common_DeleteData(this->p_this);
}

int ML_mlattribute_GetML (IMLAttribute this, unsigned int *buffer_size, char **out_buffer)
{
	return this->p_this->GetML(this->p_this,buffer_size,out_buffer);
}

/*Funcion de inicializacion de interfaz*/
void ML_mlattribute_init (IMLAttribute interfaz, ML_p_node objeto)
{
	interfaz->p_this = objeto;
	objeto->AddRef((p_com_obj)objeto);
	interfaz->AddRef = ML_mlattribute_AddRef;
	interfaz->Release = ML_mlattribute_Release;
	interfaz->QueryInterface = ML_mlattribute_QueryInterface;
	interfaz->GetType = ML_mlattribute_GetType;
	interfaz->GetName = ML_mlattribute_GetName;
	interfaz->SetName = ML_mlattribute_SetName;
	interfaz->GetNumChilds = ML_mlattribute_GetNumChilds;
	interfaz->GetSingleChild = ML_mlattribute_GetSingleChild;
	interfaz->AppendChild = ML_mlattribute_AppendChild;
	interfaz->DeleteChildByInterfaz = ML_mlattribute_DeleteChildByInterfaz;
	interfaz->DeleteChildByIndex = ML_mlattribute_DeleteChildByIndex;
	interfaz->GetDataSize = ML_mlattribute_GetDataSize;
	interfaz->GetData = ML_mlattribute_GetData;
	interfaz->SetData = ML_mlattribute_SetData;
	interfaz->DeleteData = ML_mlattribute_DeleteData;
	interfaz->GetML = ML_mlattribute_GetML;
}





