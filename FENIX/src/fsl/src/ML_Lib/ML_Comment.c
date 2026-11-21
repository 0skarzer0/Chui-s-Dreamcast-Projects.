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

#include "ML_Comment.h"
#include "ML_common_func.h"


/*************************************************************************

				Aqui trataremos los nodos Commento y su interfaz

**************************************************************************

	La estructura de este fichero es similar a la del resto de nodos.

*************************************************************************/

/**************************************************************************

						Nodos de tipo Commento

***************************************************************************/



/*	El Constructor es lo unico que cambia por ahora.
	Tanto en el Destructor como la operacion Clone se utilizan la funciones
de la clase base nodo.
*/

void ML_comment_Constructor (p_com_obj this)
{
	ML_node_Constructor(this);
	this->type=ML_NODE_Comment;
}

/*   En los Atributos las unicas interfaces disponibles son IUnknown, 
IMLNode y IMLComment.
*/

int ML_comment_QueryInterface (p_com_obj this, int IID, void **p_IObj)
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

	case IID_IMLComment:
		*p_IObj = NULL;
		*p_IObj = malloc (sizeof(struct MLComment));
		if (*p_IObj == NULL) 
		{
			/*Marcar error en sistema de definicion de errores*/
			return -3; /*No hay memoria disponible*/
		}
		
		ML_mlcomment_init((IMLComment)(*p_IObj),(ML_p_node)this);
		
		return 0; /*Todo bien*/

	default:
		/* Marcar el error en el sistema de definicion de errores*/

		return -2; /*Interfaz no reconocida*/	
	}

	return -1;
}

int ML_comment_AppendChild (ML_p_node this, ML_p_node child, ML_p_node *ret)
{
	return -2; /* No admite que se le cuelguen nodos*/
}

int ML_comment_GetML (ML_p_node this, unsigned int *buf_size, char **out_buf)
{
	if (this->size==0)  /*tiene preferencia el campo data sobre los nodos hijos*/
	{
		return -2; /*Habra que estudiar la posibilidad de mandar un buffer vacio*/
	}
	else
	{
		*out_buf=(char *) malloc (this->size+8);
		if (*out_buf==NULL) return -1;
		*buf_size=this->size+8;
		memcpy(*out_buf,"<!--",4);
		memcpy(*out_buf+3,this->data, this->size); /*no es una cadena lo que copiamos*/
		memcpy(*out_buf+this->size+4,"-->\0",4);
	}

	return 0;
}


/*Sencilla inicializacion de un nodo tipo Commento*/
void ML_comment_init (ML_p_node nodo)
{
	nodo->Constructor=ML_comment_Constructor;
	nodo->Destructor=ML_node_Destructor;
	nodo->AddRef=ML_node_AddRef;
	nodo->Release=ML_node_Release;
	nodo->QueryInterface=ML_comment_QueryInterface;
	nodo->Copy=ML_node_Clone;
	nodo->AppendChild=ML_comment_AppendChild;
	nodo->GetML=ML_comment_GetML;
	nodo->Constructor((p_com_obj)nodo);
}

/******************************************************************************
	************************* IMLComment *******************************

	Aqui comienza la interfaz IMLComment para objetos del tipo nodo_Commento
*******************************************************************************/

void ML_mlcomment_AddRef (IMLComment this)
{
	this->p_this->AddRef((p_com_obj)this->p_this);
}

void ML_mlcomment_Release (IMLComment this)
{
	this->p_this->Release((p_com_obj)this->p_this);
	 
	free (this);
}

int ML_mlcomment_QueryInterface (IMLComment this, int IID, void **p_IObj)
{
	return this->p_this->QueryInterface((p_com_obj)this->p_this,IID,p_IObj);
}

int ML_mlcomment_GetType (IMLComment this)
{
	return this->p_this->type;
}

const char *ML_mlcomment_GetName (IMLComment this)
{
	return this->p_this->name;
}

int ML_mlcomment_SetName (IMLComment this, char * name)
{
	strncpy(this->p_this->name,name,256);
	return 0;
}

int ML_mlcomment_GetDataSize (IMLComment this)
{
	return this->p_this->size;
}

int ML_mlcomment_GetData (IMLComment this, int *buffer_size, void **out_buffer)
{
	return ML_common_GetData (this->p_this, buffer_size, out_buffer);
}

int ML_mlcomment_SetData (IMLComment this, int buffer_size, void *in_buffer)
{
	return ML_common_SetData (this->p_this, buffer_size, in_buffer);
}

int ML_mlcomment_DeleteData (IMLComment this)
{
	return ML_common_DeleteData(this->p_this);
}

int ML_mlcomment_GetML (IMLComment this, unsigned int *buffer_size, char **out_buffer)
{
	return this->p_this->GetML(this->p_this,buffer_size,out_buffer);
}

/*Funcion de inicializacion de interfaz*/
void ML_mlcomment_init (IMLComment interfaz, ML_p_node objeto)
{
	interfaz->p_this = objeto;
	objeto->AddRef((p_com_obj)objeto);
	interfaz->AddRef = ML_mlcomment_AddRef;
	interfaz->Release = ML_mlcomment_Release;
	interfaz->QueryInterface = ML_mlcomment_QueryInterface;
	interfaz->GetType = ML_mlcomment_GetType;
	interfaz->GetName = ML_mlcomment_GetName;
	interfaz->SetName = ML_mlcomment_SetName;
	interfaz->GetDataSize = ML_mlcomment_GetDataSize;
	interfaz->GetData = ML_mlcomment_GetData;
	interfaz->SetData = ML_mlcomment_SetData;
	interfaz->DeleteData = ML_mlcomment_DeleteData;
	interfaz->GetML = ML_mlcomment_GetML;
}


