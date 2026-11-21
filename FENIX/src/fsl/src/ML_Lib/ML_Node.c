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

#include "ML_Node.h"
#include "ML_common_func.h"


/*
		Estas funciones pueden utilizarse en como constructores o 
	destructores de base llamandolas desde los constructores y destructores 
	de los nodos elemento o atributo...
*/

void ML_node_Constructor (p_com_obj obj)
{
	ML_p_node this;
/*Hago una conversion y asi el codigo es mas facil, a parte de no cambiar desde la version 0.1*/
	this= (ML_p_node)obj;

	this->ref=0;
	this->type=ML_NODE_0;

	strcpy(this->name,"");
	this->doc=NULL;
	this->parent=NULL;
	this->num_childs=0;
	this->childs=NULL;

	this->size=0;
	this->data=NULL;
}


void ML_node_Destructor (p_com_obj obj)
{
	int i;
	ML_p_node this; 
/*Hago una conversion y asi el codigo es mas facil, a parte de no cambiar desde la version 0.1*/
	this= (ML_p_node)obj; 

	for (i=0;i<((ML_p_node)this)->num_childs;i++)
	{
		this->childs[i]->Release((p_com_obj)this->childs[i]);
	}

	if (this->childs) free(this->childs);

	if (this->size!=0) /*quizas esto sea redundante*/
	{
		if (this->data!=NULL) free (this->data);
	}

	free(this);
	
}

/*    Estas funciones pueden ser definidas de forma diferente en cada tipo de nodo.
	Aunque no es necesario ya que la liberacion de recursos se hace en el destructor.
*/

void ML_node_AddRef (p_com_obj this)
{
	this->ref++;
}

void ML_node_Release (p_com_obj this)
{
	this->ref--;
	if (this->ref<=0) this->Destructor(this);
}


/*   Esta funcion se define en cada tipo de nodo y determina las interfaces disponibles
solo la pongo a modo ejemplo, pero es lo mas basico que puede haber en un objeto COM*/

int ML_node_QueryInterface (p_com_obj this, int IID, void **p_IObj)
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

	default:
		/* Marcar el error en el sistema de definicion de errores*/

		return -2; /*Interfaz no reconocida*/	
	}

	return -1;
}


/***************************************************************************
		Esta funcion es necesario reescribirla en las subclases en las que el
	campo data del nodo contenga algun dato referenciado (p.e. lista enlazada).
****************************************************************************/
ML_p_node ML_node_Clone (ML_p_node this)
{
	int i,j;
	ML_p_node tmp;

	tmp = (ML_p_node) malloc (sizeof(struct ML_node));
	if (tmp==NULL) return NULL;

	tmp->ref=0;
	tmp->type=this->type;

	strcpy(tmp->name,this->name);
	tmp->size=this->size;
	if (tmp->size!=0)
	{
		tmp->data=malloc(this->size);
		if (tmp->data==NULL)
		{
			free(tmp);
			return NULL;
		}
		memcpy(tmp->data,this->data,this->size);
	}
	else tmp->data=NULL;

	tmp->doc=this->doc;
	tmp->parent=NULL;

	tmp->num_childs=this->num_childs;
	if (this->num_childs>0)
	{
		tmp->childs=(ML_p_node *) malloc (this->num_childs*sizeof(ML_p_node));
		if (tmp->childs==NULL)
		{
			if (tmp->data) free (tmp->data);
			free(tmp);
			return NULL;
		}
	
		for (i=0;i<this->num_childs;i++)
		{
			tmp->childs[i]=this->childs[i]->Copy(this->childs[i]);
			if (tmp->childs[i]==NULL) /*limpiar la memoria es fundamental*/
			{
				if (tmp->data) free (tmp->data);
				for (j=0;j<i;j++)
				{
					tmp->childs[j]->Release((p_com_obj)tmp->childs[j]);
				}
				if (tmp->childs) free (tmp->childs);
				if (tmp) free(tmp);
				return NULL;
			}
			tmp->childs[i]->AddRef((p_com_obj)tmp->childs[i]);
		}
	}
	else tmp->childs=NULL;

	tmp->Constructor=this->Constructor;
	tmp->Destructor=this->Destructor;
	tmp->AddRef=this->AddRef;
	tmp->Release=this->Release;
	tmp->QueryInterface=this->QueryInterface;
	tmp->Copy=this->Copy;
	tmp->GetML=this->GetML;
	tmp->AppendChild=this->AppendChild;

	return tmp;
}


int ML_node_AppendChild (ML_p_node this, ML_p_node child, ML_p_node *ret)
{
	/*En este caso pasa todo tipo de nodo pero al redefinirla en cada tipo se filtraran los tipos validos*/
	return ML_common_AppendChild(this,child,ret);
}

/******************************************************************************
		Aqui comienza la interfaz IMLNode para objetos del tipo nodo
*******************************************************************************/
/*Funciones COM*/

void ML_mlnode_AddRef (IMLNode this)
{
	this->p_this->AddRef((p_com_obj)this->p_this);
}

void ML_mlnode_Release (IMLNode this)
{
	this->p_this->Release((p_com_obj)this->p_this);
	 
	free (this);
}

int ML_mlnode_QueryInterface (IMLNode this, int IID, void **p_IObj)
{
	return this->p_this->QueryInterface((p_com_obj)this->p_this, IID, p_IObj);
}

/*Para conocer el tipo de nodo con el que tratamos, asi podriamos solicitar
la interfaz adecuada con QueryInterface para obtener mayor fucnionalidad*/
int ML_mlnode_GetType (IMLNode this)
{
	return this->p_this->type;
}

/*Para controlar el nombre de los nodos*/
const char *ML_mlnode_GetName (IMLNode this)
{
	return this->p_this->name;
}

int ML_mlnode_SetName (IMLNode this, char * name)
{
	strncpy(this->p_this->name,name,256);
	return 0;
}

int ML_mlnode_GetNumChilds (IMLNode this)
{
	return this->p_this->num_childs;
}

/*Para manejar los nodos descendientes. En futuras versiones se añadira
soporte para XPath en funciones tipo GetChild()*/
int ML_mlnode_GetSingleChild (IMLNode this, char * name, IMLNode *out, int index)
{
	return ML_common_GetSingleChild (this->p_this, name, out, index);
}

int ML_mlnode_AppendChild (IMLNode this, IMLNode child, IMLNode *ret)
{
	ML_p_node tmp;
	int i;

	i=ML_node_AppendChild (this->p_this, child->p_this, &tmp);
	if (i>=0)
	{
		*ret=(IMLNode) malloc (sizeof (struct MLNode));
		if (*ret!=NULL)	ML_mlnode_init(*ret,tmp);
	}
	return i;	
}

int ML_mlnode_DeleteChildByInterfaz (IMLNode this, IMLNode child)
{
	return ML_common_DeleteChildByInterfaz (this->p_this, child->p_this);
}

int ML_mlnode_DeleteChildByIndex (IMLNode this, int index)
{
	return ML_common_DeleteChildByIndex (this->p_this, index);
}

int ML_mlnode_GetML (IMLNode this, unsigned int *buffer_size, char **out_buffer)
{
	return this->p_this->GetML(this->p_this,buffer_size,out_buffer);
}

void ML_mlnode_init (IMLNode interfaz, ML_p_node objeto)
{
	interfaz->p_this = objeto;
	objeto->AddRef((p_com_obj)objeto);
	interfaz->AddRef = ML_mlnode_AddRef;
	interfaz->Release = ML_mlnode_Release;
	interfaz->QueryInterface = ML_mlnode_QueryInterface;
	interfaz->GetType = ML_mlnode_GetType;
	interfaz->GetName = ML_mlnode_GetName;
	interfaz->SetName = ML_mlnode_SetName;
	interfaz->GetNumChilds = ML_mlnode_GetNumChilds;
	interfaz->GetSingleChild = ML_mlnode_GetSingleChild;
	interfaz->AppendChild = ML_mlnode_AppendChild;
	interfaz->DeleteChildByInterfaz = ML_mlnode_DeleteChildByInterfaz;
	interfaz->DeleteChildByIndex = ML_mlnode_DeleteChildByIndex;
	interfaz->GetML=ML_mlnode_GetML;
}


