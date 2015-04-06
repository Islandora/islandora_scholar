<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
 <!-- romeo XSLT -->
 <xsl:template match="/">
  <table cellpadding="3" cellspacing="3"><tr><td>
  <strong>Journal: </strong></td><td><xsl:value-of select="/romeoapi/journals/journal/jtitle"/></td></tr>
  <tr><td><strong>Published By: </strong></td><td><xsl:value-of select="/romeoapi/journals/journal/zetocpub"/><br /></td></tr>
   <tr><td><strong>Publisher Url:</strong></td><td><xsl:value-of select="/romeoapi/publishers/publisher/homeurl"/></td></tr>
   <tr><td><strong>Prearchiving:</strong></td><td><xsl:value-of select="/romeoapi/publishers/publisher/preprints/prearchiving"/></td></tr>
   <tr><td><strong>Postprints:</strong></td><td><xsl:value-of select="/romeoapi/publishers/publisher/postprints/postarchiving"/></td></tr>
  </table>
  <h2>Conditions</h2>
  <ul>
   <xsl:for-each select="romeoapi/publishers/publisher/conditions/condition">
    <li><xsl:value-of select="."/></li>
   </xsl:for-each>
  </ul>

  <span><strong>Rights: </strong><xsl:value-of select="/romeoapi/publishers/publisher/copyright/text()" disable-output-escaping="yes"/></span><br />
  <xsl:variable name="COLOR">
			<xsl:value-of select="/romeoapi/publishers/publisher/romeocolour"/>
			</xsl:variable>
  <span STYLE="background-color: light{$COLOR}"><strong>Romeo Colour: </strong><xsl:value-of select="/romeoapi/publishers/publisher/romeocolour"/></span><br />
  <div align="right"><a href="http://www.sherpa.ac.uk/romeo.php"><img src="http://www.sherpa.ac.uk/images/romeotiny.gif"/></a></div>
 	<xsl:if test = "count(/romeoapi/publishers/publisher/homeurl) 	&lt; 1">
    	<div>If there are no results from <strong>ROMEO</strong> shown above you can try searching manually by clicking <a href="http://www.sherpa.ac.uk/romeo.php" target="_blank">here</a>.</div>
	</xsl:if>
 </xsl:template>
</xsl:stylesheet>
