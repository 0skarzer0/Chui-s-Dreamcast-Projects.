<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:variable name="paramcount">0</xsl:variable>

<xsl:template match="/">
  <html>
  <head>
    <title><xsl:value-of select="//@NAME"/></title>
    <style type="text/css">
      .txtNaranja {
        font-family: Arial ;                            
        font-size: 12px ;
        font-weight: bold ;
        color: #ff6600 ;
      }
      .txtNegroBigBold {
        font-family: Arial ;
        font-size: 12px ;
        font-weight: bold ;
        color: #0000 ;
      }
      .txtNegroBig {
        font-family: Arial ;
        font-size: 12px ;
        color: #0000 ;
      }
      .txtNegro {
        font-family: Arial ;
        font-size: 10px ;
        color: #0000 ;
      }
    </style>
  </head>
  <body>
  <span class="txtNaranja"><xsl:value-of select="//@NAME"/></span><br/>
  <UL>
  <xsl:apply-templates select="//RETVAL" /><span class="txtNegroBig"><xsl:value-of select="//@NAME"/>(<xsl:apply-templates select="//PARAM" />)</span>	
  </UL>
  <xsl:apply-templates select="//ABSTRACT" />	
  </body>
  </html>
</xsl:template>

<xsl:template match="//RETVAL">
<span class="txtNegroBigBold"><xsl:value-of select="./@TYPE"/><xsl:text> </xsl:text></span>
</xsl:template>
<xsl:template match="//ABSTRACT">
<ul><xsl:value-of select="." /></ul>
</xsl:template>

<xsl:template match="//PARAM">
<span class="txtNegroBigBold"><xsl:text> </xsl:text><xsl:value-of select="./@TYPE"/><xsl:text> </xsl:text></span><span class="txtNegroBig"><xsl:value-of select="./@NAME"/></span>
<xsl:if test = "position()!=last()"><xsl:text>,</xsl:text></xsl:if>
</xsl:template>

</xsl:transform>
                