<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text"/>

<xsl:template match="/">
	<xsl:for-each select="./dblp/child::*">
		<xsl:call-template name="handlenode">
			<xsl:with-param name="curnode" select="."/>
		</xsl:call-template>
	</xsl:for-each>
</xsl:template>

<xsl:template name="handlenode">
	<xsl:param name="curnode"/>
	<xsl:if test="$curnode/author and $curnode/title">
		<xsl:for-each select="$curnode/author">
			<xsl:value-of select="concat(., ' ')" />
		</xsl:for-each>
		<xsl:value-of select="$curnode/title" /><xsl:text>
</xsl:text>

  </xsl:if>
</xsl:template>

<!--<xsl:template mode="item">
  <xsl:value-of select="name(.)" />
</xsl:template>-->

</xsl:stylesheet>
