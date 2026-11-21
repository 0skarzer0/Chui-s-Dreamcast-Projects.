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
#include "ML_common_func.h"
#include "ML_Element.h"
#include "ML_Process.h"
#include "ML_Text.h"

/*leng_process.h funciones para facilitar el procesamiento de los ficheros*/

int ML_is_reserv (char c);
int ML_salta_espacio (char *buf, int size, int *index, int *old_index);
int ML_busca_menor (char *buf, int size, int *index, int *old_index);
int ML_busca_cadena (char *buf, int size, char *cad, int *index, int *old_index);
int ML_busca_etiqueta_fin (char *buf, int size, int *index, int *old_index, char *nom_busq);
int ML_coger_nombre (char *buf, int size, int *index, int *old_index);
int ML_coger_atr_valor (char *buf, int size, int *index, int *old_index);


int ML_AnalizaE(IMLDocument doc, FILE *fichero, char *buf, int *i, int *size, IMLElement *tmp);
int ML_AnalizaPI(IMLDocument doc, FILE *fichero, char *buf, int *i, int *size, IMLProcessInstruction *tmp);
int ML_AnalizaT(IMLDocument doc, FILE *fichero, char *buf, int *i, int *size, IMLText *tmp);
int ML_AnalizaD(IMLDocument doc, FILE *fichero, char *buf, int *i, int *size, IMLText *tmp);
int ML_AnalizaC(IMLDocument doc, FILE *fichero, char *buf, int *i, int *size, IMLComment *tmp);

