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

#include "ML_common_func.h"

int ML_common_GetSingleChild (ML_p_node this, char * name, IMLNode *out, int index)
{
	int i=index;

	for (i;i<this->num_childs;i++)
	{
		if (strcmp(this->childs[i]->name,name)==0)
		{
			*out=(IMLNode) malloc (sizeof(struct MLNode));
			ML_mlnode_init(*out,this->childs[i]);
			return i;
		}
	}

	return -1;
}

void ML_common_ActualizaSourceDoc (ML_p_node tmp, ML_p_document doc)
{
	int i;

	for (i=0; i<tmp->num_childs;i++)
	{
		ML_common_ActualizaSourceDoc(tmp->childs[i],doc);
	}
	tmp->doc=doc;
}

int ML_common_AppendChild (ML_p_node this, ML_p_node child, ML_p_node *ret)
{
	int i;
	ML_p_node *tmp1;

	(*ret) = child->Copy(child);
	if (*ret==NULL)
	{
		return -1;
	}
	ML_common_ActualizaSourceDoc((*ret),this->doc);
	(*ret)->parent=this;

	tmp1 = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->num_childs+1));

	if (tmp1==NULL) 
	{
		(*ret)->Release((p_com_obj)(*ret));
		(*ret)=NULL;
		return -1; /*por si no hay memoria*/
	}

	memcpy(tmp1,this->childs,sizeof(ML_p_node)*this->num_childs);
	tmp1[this->num_childs]=(*ret);

	(*ret)->AddRef((p_com_obj)(*ret));
	if (this->childs) free(this->childs);
	this->childs=tmp1;
	this->num_childs++;

	return i;
}

int ML_common_AddChild (ML_p_node this, ML_p_node child)
{
	int i;
	ML_p_node *tmp1;

	tmp1 = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->num_childs+1));

	if (tmp1==NULL) 
	{
		return -1; /*por si no hay memoria*/
	}

	memcpy(tmp1,this->childs,sizeof(ML_p_node)*(this->num_childs));
	tmp1[this->num_childs]=child;
	child->AddRef((p_com_obj)child);
	ML_common_ActualizaSourceDoc(child,this->doc);
	child->parent=this;

	if (this->childs) free(this->childs);
	this->childs=tmp1;
	this->num_childs++;

	return i;
}

int ML_common_DocAddChild (ML_p_document this, ML_p_node child)
{
	int i;
	ML_p_node *tmp1;

	tmp1 = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->num_childs+1));

	if (tmp1==NULL) 
	{
		return -1; /*por si no hay memoria*/
	}

	memcpy(tmp1,this->childs,sizeof(ML_p_node)*(this->num_childs));
	tmp1[this->num_childs]=child;
	child->AddRef((p_com_obj)child);
	ML_common_ActualizaSourceDoc(child,this);
	child->parent=NULL;

	if (this->childs) free(this->childs);
	this->childs=tmp1;
	this->num_childs++;

	return i;
}


int ML_common_SetChild (ML_p_node this, ML_p_node child, ML_p_node *ret)
{
	int i;

	for (i=0;i<this->num_childs;i++)
	{
		if ((this->childs[i]->type==child->type)&&(strcmp(this->childs[i]->name,child->name)==0))
		{
			(*ret) = child->Copy(child);
			if ((*ret)==NULL)
			{
				return -1;
			}
			ML_common_ActualizaSourceDoc((*ret),this->doc);
			(*ret)->parent=this;

			this->childs[i]->Release((p_com_obj)this->childs[i]); /*liberamos el nodo que estaba allí*/
			this->childs[i]=(*ret);
			(*ret)->AddRef((p_com_obj)(*ret));
			return i;
		}
	}

	return ML_common_AppendChild (this, child, ret);
}


int ML_common_DeleteChildByInterfaz (ML_p_node this, ML_p_node child)
{
	int i,j,index;
	ML_p_node *tmp;

	for (index=0;index<this->num_childs;i++)
	{
		if (this->childs[index]==child)
			break;
	}

	if (index==this->num_childs)
		return -2;
	else
	{
		/*liberamos el enlace del objeto y si es el ultimo este se autodestruira*/
		this->childs[index]->Release((p_com_obj)this->childs[index]);

		/*Creamos la nueva lista de punteros de objeto*/
		tmp = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->num_childs-1));


		if (tmp==NULL) return -1; /*por si no hay memoria*/

		j=0;
		for (i=0;i<this->num_childs;i++)
		{
			if (i!=index)
				tmp[j++]=this->childs[i];
		}

		free (this->childs);/*borramos la lista vieja*/
		this->childs=tmp;/*ponemos la lista nueva*/
		this->num_childs--;
		return 0;
	}
}


int ML_common_DeleteChildByIndex (ML_p_node this, int index)
{
	int i,j;
	ML_p_node *tmp;

	if ((index>=this->num_childs)||(index<0))
		return -2;
	else
	{
		/*liberamos el enlace del objeto y si es el ultimo este se autodestruira*/
		this->childs[index]->Release((p_com_obj)this->childs[index]);

		/*Creamos la nueva lista de punteros de objeto*/
		tmp = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->num_childs-1));


		if (tmp==NULL) return -1; /*por si no hay memoria*/

		j=0;
		for (i=0;i<this->num_childs;i++)
		{
			if (i!=index)
				tmp[j++]=this->childs[i];
		}

		free (this->childs);/*borramos la lista vieja*/
		this->childs=tmp;/*ponemos la lista nueva*/
		this->num_childs--;
		return 0;
	}
}

int ML_common_DeleteChildByName (ML_p_node this, char *name, int type)
{
	int i,j,k;
	ML_p_node *tmp;

	for (i=0;i<this->num_childs;i++)
	{
		if ((this->childs[i]->type==type)&&(strcmp(this->childs[i]->name,name)==0))
		{
			/*Creamos la nueva lista de punteros de objeto*/
			tmp = (ML_p_node *) malloc (sizeof(ML_p_node)*(this->num_childs-1));
			if (tmp==NULL) return -1; /*por si no hay memoria*/

			/*liberamos el enlace del objeto y si es el ultimo este se autodestruira*/
			this->childs[i]->Release((p_com_obj)this->childs[i]);

			j=0;
			for (k=0;k<this->num_childs;k++)
			{
				if (k!=i)
					tmp[j++]=this->childs[k];
			}

			free (this->childs);/*borramos la lista vieja*/
			this->childs=tmp;/*ponemos la lista nueva*/
			this->num_childs--;
			return 0;
		}
	}

	return -2;
}


int ML_common_GetData (ML_p_node this, int *buffer_size, void **out_buffer)
{
	if (this->size)
	{
		*buffer_size=this->size;
		*out_buffer=malloc(this->size+1);
		if (*out_buffer==NULL) return -1; /*no hay memoria*/
		memcpy(*out_buffer,this->data,this->size);
		((char *)*out_buffer)[this->size]='\0'; /*poco ortodoxo pero efectivo ;P*/
		return 0;
	}
	return -2;
}


int ML_common_SetData (ML_p_node this, int buffer_size, void *in_buffer)
{
	if (this->size)
	{
		free(this->data);
	}

	this->size=buffer_size;
	this->data=malloc(this->size);
	if (this->data==NULL) {this->size=0; return -1;} /*no hay memoria*/
	memcpy(this->data,in_buffer,buffer_size);
	return 0;
}

int ML_common_DeleteData (ML_p_node this)
{
	if (this->size)
	{
		free(this->data);
		this->size=0;
		return 0;
	}
	else return -1;
}
