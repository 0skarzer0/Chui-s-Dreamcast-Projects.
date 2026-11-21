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

#include "ML_Process.h"
#include "ML_common_func.h"

/*************************************************************************

				Aqui trataremos los nodos proceso y su interfaz

**************************************************************************

/**************************************************************************
						Nodos de tipo proceso
***************************************************************************/



/*	El Constructor es lo unico que cambia por ahora.
	Tanto en el Destructor como la operacion Clone se utilizan la funciones
de la clase base nodo.
*/

void ML_process_Constructor (p_com_obj this)
{
	ML_node_Constructor(this);
	this->type=ML_NODE_ProcessInstruction;
}

/*   En los processos las unicas interfaces disponibles son IUnknown, 
IMLNode y IMLProcessInstruction.
*/

int ML_process_QueryInterface (p_com_obj this, int IID, void **p_IObj)
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

	case IID_IMLProcessInstruction:
		*p_IObj = NULL;
		*p_IObj = malloc (sizeof(struct MLProcessInstruction));
		if (*p_IObj == NULL) 
		{
			/*Marcar error en sistema de definicion de errores*/
			return -3; /*No hay memoria disponible*/
		}
		
		ML_mlprocess_init((IMLProcessInstruction)(*p_IObj),(ML_p_node)this);
		
		return 0; /*Todo bien*/

	default:
		/* Marcar el error en el sistema de definicion de errores*/

		return -2; /*Interfaz no reconocida*/	
	}

	return -1;
}

int ML_process_AppendChild (ML_p_node this, ML_p_node child, ML_p_node *ret)
{
	switch (child->type)
	{
	case ML_NODE_Attribute: /*no puede haber dos atributos con el mismo nombre*/
		return ML_common_SetChild(this,child,ret);

	default:
		return -2;
	}
}

int ML_process_GetML (ML_p_node this, unsigned int *buf_size, char **out_buf)
{
	int i,num_attr;
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

	num_attr=0; /*este sera el indice para la el codigo de cada hijo*/
	total_size=0;

	/*Recogemos los atributos, no puede haber otra cosa*/
	for (i=0;i<this->num_childs;i++)
	{
		if (this->childs[i]->type==ML_NODE_Attribute)
		{
			if ( -1 == ( this->childs[i]->GetML( this->childs[i], &child_size, &(buf_list[num_attr]) ) ) )
			{
				for (i=0; i<num_attr;i++)
				{
					if (buf_list[i]) free (buf_list[i]);
				}
				free (buf_list);
				return -1;
			}
			num_attr++;
			total_size+=child_size-1;
		}
	}

	/*Ya tenemos el tamaño que debera tener el buffer de salida*/
	/*el tamaño del nombre + 5 para la etiqueta y num_attr es por los espacios entre atributos*/
	total_size+=strlen(this->name)+5+num_attr;	

	*out_buf=(char *) malloc (total_size);
	if (*out_buf==NULL)
	{
		for (i=0; i<num_attr;i++)
		{
			if (buf_list[i]) free (buf_list[i]);
		}
		if (buf_list) free (buf_list);
		return -1;
	}
	
	strcpy(*out_buf,"<?");
	strcat(*out_buf,this->name);
	for (i=0;i<num_attr;i++)
	{
		strcat(*out_buf," ");
		strcat(*out_buf,buf_list[i]);
	}
	strcat(*out_buf,"?>");
	
	*buf_size=total_size;

	/*Hay que liberar la memoria utilizada en el proceso*/
	for (i=0; i<num_attr;i++)
	{
		if (buf_list[i]) free (buf_list[i]);
	}
	if (buf_list) free (buf_list);

	return 0;
}

/*Sencilla inicializacion de un nodo tipo processo*/
void ML_process_init (ML_p_node nodo)
{
	nodo->Constructor=ML_process_Constructor;
	nodo->Destructor=ML_node_Destructor;
	nodo->AddRef=ML_node_AddRef;
	nodo->Release=ML_node_Release;
	nodo->QueryInterface=ML_process_QueryInterface;
	nodo->Copy=ML_node_Clone;
	nodo->AppendChild=ML_process_AppendChild;
	nodo->GetML=ML_process_GetML;
	nodo->Constructor((p_com_obj)nodo);
}

/******************************************************************************
	************************* IMLProcessInstruction *******************************

	Aqui comienza la interfaz IMLProcessInstruction para objetos del tipo nodo_processo
*******************************************************************************/

void ML_mlprocess_AddRef (IMLProcessInstruction this)
{
	this->p_this->AddRef((p_com_obj)this->p_this);
}

void ML_mlprocess_Release (IMLProcessInstruction this)
{
	this->p_this->Release((p_com_obj)this->p_this);
	 
	free (this);
}

int ML_mlprocess_QueryInterface (IMLProcessInstruction this, int IID, void **p_IObj)
{
	return this->p_this->QueryInterface((p_com_obj)this->p_this,IID,p_IObj);
}

int ML_mlprocess_GetType (IMLProcessInstruction this)
{
	return this->p_this->type;
}

const char *ML_mlprocess_GetName (IMLProcessInstruction this)
{
	return this->p_this->name;
}

int ML_mlprocess_SetName (IMLProcessInstruction this, char * name)
{
	strncpy(this->p_this->name,name,256);
	return 0;
}

int ML_mlprocess_GetNumChilds (IMLProcessInstruction this)
{
	return this->p_this->num_childs;
}

int ML_mlprocess_GetSingleChild (IMLProcessInstruction this, char * name, IMLNode *out, int index)
{
	return ML_common_GetSingleChild (this->p_this, name, out, index);
}

int ML_mlprocess_AppendChild (IMLProcessInstruction this, IMLNode child, IMLNode *ret)
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

int ML_mlprocess_DeleteChildByInterfaz (IMLProcessInstruction this, IMLNode child)
{
	return ML_common_DeleteChildByInterfaz (this->p_this, child->p_this);
}

int ML_mlprocess_DeleteChildByIndex (IMLProcessInstruction this, int index)
{
	return ML_common_DeleteChildByIndex (this->p_this, index);
}


int ML_mlprocess_GetAttribute (IMLProcessInstruction this, char * name, IMLAttribute *out)
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

int ML_mlprocess_SetAttribute (IMLProcessInstruction this, IMLAttribute in)
{
	ML_p_node tmp;
	
	return ML_common_SetChild(this->p_this,in->p_this,&tmp);
}

int ML_mlprocess_DeleteAttribute (IMLProcessInstruction this, char *name)
{
	return ML_common_DeleteChildByName(this->p_this,name,ML_NODE_Attribute);
}

/*todavia no se si utilizare el campo data*/
unsigned int ML_mlprocess_GetDataSize (IMLProcessInstruction this)
{
	return this->p_this->size;
}

int ML_mlprocess_GetData (IMLProcessInstruction this, unsigned int *buffer_size, void **out_buffer)
{
	return ML_common_GetData (this->p_this, buffer_size, out_buffer);
}

int ML_mlprocess_SetData (IMLProcessInstruction this, unsigned int buffer_size, void *in_buffer)
{
	return ML_common_SetData (this->p_this, buffer_size, in_buffer);
}

int ML_mlprocess_DeleteData (IMLProcessInstruction this)
{
	return ML_common_DeleteData(this->p_this);
}

int ML_mlprocess_GetML (IMLProcessInstruction this, unsigned int *buffer_size, char **out_buffer)
{
	return this->p_this->GetML(this->p_this,buffer_size,out_buffer);
}

/*Funcion de inicializacion de interfaz*/
void ML_mlprocess_init (IMLProcessInstruction interfaz, ML_p_node objeto)
{
	interfaz->p_this = objeto;
	objeto->AddRef((p_com_obj)objeto);
	interfaz->AddRef = ML_mlprocess_AddRef;
	interfaz->Release = ML_mlprocess_Release;
	interfaz->QueryInterface = ML_mlprocess_QueryInterface;
	interfaz->GetType = ML_mlprocess_GetType;
	interfaz->GetName = ML_mlprocess_GetName;
	interfaz->SetName = ML_mlprocess_SetName;
	interfaz->GetNumChilds = ML_mlprocess_GetNumChilds;
	interfaz->GetSingleChild = ML_mlprocess_GetSingleChild;
	interfaz->AppendChild = ML_mlprocess_AppendChild;
	interfaz->DeleteChildByInterfaz = ML_mlprocess_DeleteChildByInterfaz;
	interfaz->DeleteChildByIndex = ML_mlprocess_DeleteChildByIndex;
	interfaz->GetAttribute = ML_mlprocess_GetAttribute;
	interfaz->SetAttribute = ML_mlprocess_SetAttribute;
	interfaz->DeleteAttribute = ML_mlprocess_DeleteAttribute;
	interfaz->GetDataSize = ML_mlprocess_GetDataSize;
	interfaz->GetData = ML_mlprocess_GetData;
	interfaz->SetData = ML_mlprocess_SetData;
	interfaz->DeleteData = ML_mlprocess_DeleteData;
	interfaz->GetML = ML_mlprocess_GetML;
}



