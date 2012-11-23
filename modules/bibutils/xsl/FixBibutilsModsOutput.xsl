<?xml version="1.0" encoding="UTF-8"?>
<!--
    Document   : FixBibutilsModsOutput.xsl
    Created on : March 23, 2011, 3:19 PM
    Author     : nbanks
    Description:
    Transforms the invalid mods that Bibutils generates into valid v1.1 MODS.
-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:mods="http://www.loc.gov/mods/v3">
    <xsl:output omit-xml-declaration="yes" indent="yes" method="xml"/>
    <xsl:strip-space elements="*"/>
    <xsl:template match="/mods:modsCollection/mods:mods | @* | node()[local-name() != 'modsCollection']">
        <xsl:copy>
            <xsl:apply-templates select="@* | node()"/>
        </xsl:copy>
    </xsl:template>

    <!-- Move the language into the proper languageTerm field -->
    <xsl:template match="mods:mods/mods:language">
        <xsl:copy>
            <xsl:apply-templates select="@*"/>
            <xsl:element name="languageTerm" namespace="http://www.loc.gov/mods/v3">
                <xsl:value-of select="current()"/>
            </xsl:element>
        </xsl:copy>
    </xsl:template>

    <xsl:template match="mods:mods/mods:part">
      <xsl:copy>
        <xsl:apply-templates select="@* | *"/>
        <!-- If we don't have a date, grab it from the originInfo/dateIssued -->
        <xsl:variable name="date" select="(//mods:mods/mods:originInfo)[1]"/>
        <xsl:if test="not(mods:date) and $date">
          <xsl:element name="date" namespace="http://www.loc.gov/mods/v3">
            <!-- maintain the same attributes -->
            <xsl:apply-templates select="$date/@*"/>
            <xsl:value-of select="$date/text()"/>
          </xsl:element>
        </xsl:if>
      </xsl:copy>
    </xsl:template>

    <xsl:template match="mods:mods/mods:name[@type='personal']">
      <xsl:copy>
        <xsl:apply-templates select="@* | mods:namePart[not(@type='given' or @type='family')] | mods:displayForm | mods:affiliation | mods:role | mods:description"/>
        <xsl:call-template name="namepart">
          <xsl:with-param name="type">given</xsl:with-param>
        </xsl:call-template>
        <xsl:call-template name="namepart">
          <xsl:with-param name="type">family</xsl:with-param>
        </xsl:call-template>
      </xsl:copy>
    </xsl:template>
  
    <xsl:template name="namepart">
      <xsl:param name="type"/>

      <xsl:variable name="parts" select="mods:namePart[@type=$type]"/>
      <xsl:if test="$parts">
        <xsl:element name="namePart" namespace="http://www.loc.gov/mods/v3">
          <xsl:attribute name="type">
            <xsl:value-of select="$type"/>
          </xsl:attribute>
          <xsl:for-each select="$parts">
            <xsl:variable name="text" select="normalize-space(text())"/>
            <xsl:if test="$text">
              <xsl:if test="position() > 1"><xsl:text> </xsl:text></xsl:if>
              <xsl:value-of select="$text"/>
              <xsl:if test="string-length($text) = 1"><xsl:text>.</xsl:text></xsl:if>
            </xsl:if>
          </xsl:for-each>
        </xsl:element>
      </xsl:if>
    </xsl:template>
</xsl:stylesheet>
