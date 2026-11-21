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

#if !defined(ML_COMMON_FUNC_H)
#define ML_COMMON_FUNC_H

#include "ML_Node.h"
#include "ML_Element.h"
#include "ML_Process.h"
#include "ML_Attribute.h"
#include "ML_Text.h"
#include "ML_Comment.h"
#include "ML_Document.h"

int ML_common_GetSingleChild (ML_p_node this, char * name, IMLNode *out, int index);

void ML_common_ActualizaSourceDoc (ML_p_node tmp, ML_p_document doc);

int ML_common_AppendChild (ML_p_node this, ML_p_node child, ML_p_node *ret);

int ML_common_AddChild (ML_p_node this, ML_p_node child);

int ML_common_DocAddChild (ML_p_document this, ML_p_node child);

int ML_common_SetChild (ML_p_node this, ML_p_node child, ML_p_node *ret);

int ML_common_DeleteChildByInterfaz (ML_p_node this, ML_p_node child);

int ML_common_DeleteChildByIndex (ML_p_node this, int index);

int ML_common_DeleteChildByName (ML_p_node this, char *name, int type);

int ML_common_GetData (ML_p_node this, int *buffer_size, void **out_buffer);

int ML_common_SetData (ML_p_node this, int buffer_size, void *in_buffer);

int ML_common_DeleteData (ML_p_node this);

#endif /*!define(ML_COMMON_FUNC_H)*/

