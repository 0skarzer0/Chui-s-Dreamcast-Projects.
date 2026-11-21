/*
 * Fenix SDL Loader
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#include "fsl.h"
#include "ML_Lib.h"

static IMLDocument doc=NULL;

char *fsl_unknow="Unkonw";
char *fsl_anonymous="Anonymous";
char *fsl_no_description="NO DESCRIPTION";
char *fsl_empty="";

#define STR_FENIX	"fenix"
#define STR_VERSION	"version"
#define STR_NVER	"0.84"
#define STR_GAMES	"games"
#define STR_NAME	"name"
#define STR_GAME	"game"
#define STR_TYPE	"type"
#define STR_EXEC	"exec"
#define STR_AUTHOR	"author"
#define STR_IMAGE	"image"
#define STR_DESCRIPTION	"description"

#define STR_TYPE_DCB	"DCB"
#define STR_TYPE_BIN	"BIN"


char *fsl_games_name;


static int openDoc(char *name)
{
	IMLProcessInstruction pi=NULL;
	IMLAttribute pa=NULL;
	int i,ret=0;
	char *tmp_str;

	DocumentFromCOM(IID_IMLDocument,&doc);
	if(! doc->LoadFile(doc,name))
	{
		i=doc->GetSingleProcessInstruction(doc,STR_FENIX,&pi,0);
		if (i>=0)
		{
			if (pi->GetAttribute(pi,STR_VERSION,&pa)>=0)
			{
				if (pa->GetData(pa,&i,(void *)&tmp_str)>=0)
				{
					if (i==strlen(STR_NVER))
					{
						if (strncmp(tmp_str,STR_NVER,i))
							ret=-5;
					} else ret=-5;
				} else ret=-4;
			} else ret=-3;
		} else ret=-2;
	} else ret=-1;

	if (pa)
		pa->Release(pa);
	if (pi)
		pi->Release(pi);
	if (ret && doc)
	{
		doc->Release(doc);
		doc=NULL;
	}
	if (tmp_str)
		free(tmp_str);
	return ret;
}

static void closeDoc(void)
{
	if (doc)
	{
		doc->Release(doc);
		doc=NULL;
	}
}

static p_fsl_game getNewGame(void)
{
	p_fsl_game actual,next,g=calloc(sizeof(fsl_game),1);

	if (g!=NULL)
	{	
		actual=NULL;
		next=fsl_first;
		while(next)
		{
			actual=next;
			next=(p_fsl_game)actual->next;
		}
		if (actual)
		{
			actual->next=(void *)g;
			g->back=(void *)actual;
		}
		else
			fsl_first=g;

		g->name=fsl_unknow;
		g->author=fsl_anonymous;
		g->description=fsl_no_description;
		g->image=fsl_dcimage;
		g->exec=fsl_empty;
	}
	return g;
}

static char* getElement(IMLElement elem, char *search, int *i, int sensitive)
{
	int ret=0;
	IMLAttribute attr=NULL;
	char *tmp_str=NULL;
	int n;
	
	if (elem->GetAttribute(elem,search,&attr)>=0)
	{
		if (attr->GetData(attr,i,(void *)&tmp_str)>=0)
		{
			tmp_str[*i]=0;
			if (!sensitive)
			for(n=0;n<*i;n++)
				if (tmp_str[n]>='a' && tmp_str[n]<='z')
					tmp_str[n]-=('a'-'A');
			ret=-1;
		}
		else
			if (tmp_str)
			{
				free(tmp_str);
				tmp_str=NULL;
			}
	}
	if (attr)
		attr->Release(attr);
	return tmp_str;
}

static SDL_Surface *getImage(char *filename)
{
	SDL_Surface *tmp, *ret=NULL;
	char *real_filename=calloc(1024,1);
	sprintf(real_filename,"%s%s",DATA_PREFIX,filename);
	tmp=IMG_Load(real_filename);
	free(real_filename);
	if (tmp!=NULL)
	{
		ret=SDL_ConvertSurface(tmp,fsl_screen->format,SDL_RLEACCEL);
		SDL_FreeSurface(tmp);
	}
	free(filename); // tmp_str
	if (!ret)
		ret=fsl_dcimage;
	return ret;
}

static int parseDoc(void)
{	
	IMLElement elem=NULL,subelem=NULL;
	IMLAttribute attr=NULL;
	int index=0,i,max,ret=0;
	p_fsl_game g;
	char *tmp_str=NULL;

	if (doc)
	{
	 if (doc->GetSingleElement(doc,STR_GAMES,&elem,0)>=0)
	 {
	  if (elem->GetAttribute(elem,STR_NAME,&attr)>=0)
	  {
	   if (attr->GetData(attr,&i,(void *)&fsl_games_name)>=0)
	   {
	    fsl_set_window_title();
	    tmp_str=NULL;
	    attr->Release(attr);
	    attr=NULL;
	    max=elem->GetNumChilds(elem);
	    while((index=elem->GetSingleElement(elem,STR_GAME,&subelem,index))>=0)
	    {
		attr=NULL;
		if ((g=getNewGame())<0)
			return -1;
		if ((tmp_str=getElement(subelem,STR_TYPE,&i,0))!=NULL)
		{
			if (!strcmp(tmp_str,STR_TYPE_DCB))
				g->type=FSL_EXEC_TYPE_DCB;
			else
			if (!strcmp(tmp_str,STR_TYPE_BIN))
				g->type=FSL_EXEC_TYPE_BIN;
			free(tmp_str);
		}
		if ((tmp_str=getElement(subelem,STR_NAME,&i,1))!=NULL)
			g->name=tmp_str;
		if ((tmp_str=getElement(subelem,STR_AUTHOR,&i,1))!=NULL)
			g->author=tmp_str;
		if ((tmp_str=getElement(subelem,STR_DESCRIPTION,&i,1))!=NULL)
			g->description=tmp_str;
		if ((tmp_str=getElement(subelem,STR_EXEC,&i,0))!=NULL)
			g->exec=tmp_str;
		if ((tmp_str=getElement(subelem,STR_IMAGE,&i,0))!=NULL)
			g->image=getImage(tmp_str);
	        tmp_str=NULL;
		index++;
		if (subelem)
			subelem->Release(subelem);
		subelem=NULL;
	    }
	    if (subelem)
		subelem->Release(subelem);
	    subelem=NULL;
           } else ret=-4;
	  } else ret=-3;
	 } else ret=-2;
	} else ret=-1;
	
	if (attr)
		attr->Release(attr);
	if (subelem)
		subelem->Release(subelem);
	if (elem)
		elem->Release(elem);
	if (tmp_str)
		free(tmp_str);
	return ret;
}




int fsl_parse_game(void)
{
	int ret=0;
	if (!openDoc(DATA_PREFIX "fenix.xml"))
	{
		if (parseDoc())
			ret=-1;
	}
	else
		ret=-2;
	closeDoc();
	return ret;
}

