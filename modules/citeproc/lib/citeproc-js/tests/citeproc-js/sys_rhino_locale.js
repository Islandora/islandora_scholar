/*
 * Copyright (c) 2009, 2010 and 2011 Frank G. Bennett, Jr. All Rights
 * Reserved.
 *
 * The contents of this file are subject to the Common Public
 * Attribution License Version 1.0 (the “License”); you may not use
 * this file except in compliance with the License. You may obtain a
 * copy of the License at:
 *
 * http://bitbucket.org/fbennett/citeproc-js/src/tip/LICENSE.
 *
 * The License is based on the Mozilla Public License Version 1.1 but
 * Sections 14 and 15 have been added to cover use of software over a
 * computer network and provide for limited attribution for the
 * Original Developer. In addition, Exhibit A has been modified to be
 * consistent with Exhibit B.
 *
 * Software distributed under the License is distributed on an “AS IS”
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is the citation formatting software known as
 * "citeproc-js" (an implementation of the Citation Style Language
 * [CSL]), including the original test fixtures and software located
 * under the ./std subdirectory of the distribution archive.
 *
 * The Original Developer is not the Initial Developer and is
 * __________. If left blank, the Original Developer is the Initial
 * Developer.
 *
 * The Initial Developer of the Original Code is Frank G. Bennett,
 * Jr. All portions of the code written by Frank G. Bennett, Jr. are
 * Copyright (c) 2009, 2010 and 2011 Frank G. Bennett, Jr. All Rights Reserved.
 *
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU Affero General Public License (the [AGPLv3]
 * License), in which case the provisions of [AGPLv3] License are
 * applicable instead of those above. If you wish to allow use of your
 * version of this file only under the terms of the [AGPLv3] License
 * and not to allow others to use your version of this file under the
 * CPAL, indicate your decision by deleting the provisions above and
 * replace them with the notice and other provisions required by the
 * [AGPLv3] License. If you do not delete the provisions above, a
 * recipient may use your version of this file under either the CPAL
 * or the [AGPLv3] License.”
 */

dojo.provide("citeproc_js.sys_rhino_locale");

doh.register("citeproc_js.sys_rhino_locale", [

	function testGetTermSymbolFallbackToShort(){
		var sys = new RhinoTest();
		var obj = new CSL.Engine(sys,"<style></style>");
		var res = CSL.Engine.prototype.getTerm.call(obj,"edition","symbol");
		doh.assertEqual("ed.",res);
	},
	function testGetTermNoPluralSpecifiedFallbackToSingular(){
		var sys = new RhinoTest();
		var obj = new CSL.Engine(sys,"<style></style>");
		var res = CSL.Engine.prototype.getTerm.call(obj,"book","long");
		doh.assertEqual("book",res);
	},
	function testGetTermSingularExists(){
		var sys = new RhinoTest();
		var obj = new CSL.Engine(sys,"<style></style>");
		var res = CSL.Engine.prototype.getTerm.call(obj,"book","long",0);
		doh.assertEqual("book",res);
	},
	function testGetTermPluralExists(){
		var sys = new RhinoTest();
		var obj = new CSL.Engine(sys,"<style></style>");
		var res = CSL.Engine.prototype.getTerm.call(obj,"book","long",1);
		doh.assertEqual("books",res);
	},
	function testSetAccess(){
		var sys = new RhinoTest();
		var obj = new CSL.Engine(sys,"<style></style>");
		var myxml = sys.xml.makeXml( sys.retrieveLocale("af-ZA") );
		obj.localeSet(myxml,"af-ZA","af-ZA");
		var myxml = sys.xml.makeXml( sys.retrieveLocale("de-DE") );
		obj.localeSet(myxml,"de-DE","de-DE");
		doh.assertEqual("und", obj.locale["de-DE"].terms["and"]["long"]);
	},
	function testSetLocaleNilValueNoStyleDefault(){
		var sys = new RhinoTest();
		var obj = new CSL.Engine(sys,"<style></style>");
		doh.assertEqual("en-US", obj.opt["default-locale"][0]);
		doh.assertEqual("books", obj.locale["en-US"].terms["book"]["long"][1]);
	},
	function testSetLocaleNilValueStyleHasDefault(){
		var sys = new RhinoTest();
		var obj = new CSL.Engine(sys,"<style default-locale=\"de-DE\"></style>");
		doh.assertEqual("de-DE", obj.opt["default-locale"][0]);
		doh.assertEqual("Bücher", obj.locale["de-DE"].terms["book"]["long"][1]);
	},
	function testSetLocaleHasValueNoStyleDefault(){
		var sys = new RhinoTest();
		var obj = new CSL.Engine(sys,"<style></style>", "de-DE");
		doh.assertEqual("de-DE", obj.opt["default-locale"][0]);
		doh.assertEqual("Bücher", obj.locale["de-DE"].terms["book"]["long"][1]);
	},
	function testSetLocaleHasValueAndStyleDefault(){
		var sys = new RhinoTest();
		var obj = new CSL.Engine(sys,"<style default-locale=\"de-AT\"></style>", "de-DE");
		doh.assertEqual("de-AT", obj.opt["default-locale"][0]);
		// Odd that this should be the value for Austrian, but that's
		// what the current de-AT locale reports.
		doh.assertEqual("books", obj.locale["de-AT"].terms["book"]["long"][1]);
	},
	function testSetLocaleHasValueAndStyleDefaultWithForceValue(){
		var sys = new RhinoTest();
		var obj = new CSL.Engine(sys,"<style default-locale=\"de-AT\"></style>", "de-DE", true);
		doh.assertEqual("de-DE", obj.opt["default-locale"][0]);
		doh.assertEqual("Bücher", obj.locale["de-DE"].terms["book"]["long"][1]);
	},
	function testSetLocaleUnknownLocaleForced(){
		var sys = new RhinoTest();
		var obj = new CSL.Engine(sys,"<style default-locale=\"xx-XX\"></style>", "yy-YY", true);
		doh.assertEqual("en-US", obj.opt["default-locale"][0]);
		doh.assertEqual("books", obj.locale["en-US"].terms["book"]["long"][1]);
	},
	function testSetLocaleUnknownLocaleOnStyle(){
		var sys = new RhinoTest();
		var obj = new CSL.Engine(sys,"<style default-locale=\"xx-XX\"></style>");
		doh.assertEqual("en-US", obj.opt["default-locale"][0]);
		doh.assertEqual("books", obj.locale["en-US"].terms["book"]["long"][1]);
	},
	function testLocalSetLocaleWorksAtAll(){
		try {
			var sys = new RhinoTest();
			var obj = new CSL.Engine(sys,"<style></style>");
			var myxml = sys.xml.makeXml( sys.retrieveLocale("de-DE") );
			obj.localeSet(myxml,"de-DE","de-DE");
			var res = "Success";
		} catch (e){
			var res = e;
		}
		doh.assertEqual("Success", res);
		doh.assertEqual("object", typeof obj.locale["de-DE"].terms);
	}
]);
