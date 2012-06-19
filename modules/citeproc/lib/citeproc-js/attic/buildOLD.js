/*
 * Copyright (c) 2009 and 2010 Frank G. Bennett, Jr. All Rights Reserved.
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
 * Copyright (c) 2009 and 2010 Frank G. Bennett, Jr. All Rights Reserved.
 */

dojo.provide("citeproc_js.build");


var nestedsingleton = "<style><text/></style>";
var complex = "<style><textA/><textB/><choose><if><textC/></if><else><textD/><textE/></else></choose></style>";

var Item = {
	"title":"My Aunt Sally"
};

doh.register("citeproc_js.build", [

	function testBuild(){
		var builder = new CSL.Core.Build(nestedsingleton);
		function testme(){
			try {
				var sys = new RhinoTest();
				builder.build(sys);
				return "Success";
			} catch(e) {
				return e;
			}
		}
		var res = testme();
		doh.assertEqual("Success", res);
	},
	function testExistence() {
		function testme () {
			try {
				var obj = CSL.Core.Build;
				return "Success";
			} catch (e) {
				return e;
			}
		}
		var res = testme();
		doh.assertEqual( "Success", res );
	},

	function testInstantiation() {
		function testme () {
			try {
				var obj = new CSL.Core.Build(nestedsingleton);
				return "Success";
			} catch (e) {
				return e;
			}
		}
		var res = testme();
		doh.assertEqual( "Success", res );
	},

	function testXmlParseOkJS(){
		var obj = new CSL.Core.Build(nestedsingleton,"JunkyardJavascript");
		var cmd = CSL.System.Xml.JunkyardJavascript.commandInterface;
		function testme (){
			try {
				var name = cmd.nodename.call(cmd.children.call(obj.showXml())[0][0]);
				return name;
			} catch (e) {
				return e;
			}
		}
		var res = testme();
		doh.assertEqual("text", res);
	},

	function testOptGetsCreated (){
		var sys = new RhinoTest();
		var builder = new CSL.Core.Build(nestedsingleton);
		var obj = builder.build(sys);
		doh.assertTrue( obj.opt );
	},
	function testCitationGetsCreated (){
		var sys = new RhinoTest();
		var builder = new CSL.Core.Build(nestedsingleton);
		var obj = builder.build(sys);
		doh.assertTrue( obj.citation );
	},
	function testBibliographyGetsCreated (){
		var sys = new RhinoTest();
		var builder = new CSL.Core.Build(nestedsingleton);
		var obj = builder.build(sys);
		doh.assertTrue( obj.bibliography );
	},
	function testCitationOptGetsCreated (){
		var sys = new RhinoTest();
		var builder = new CSL.Core.Build(nestedsingleton);
		var obj = builder.build(sys);
		doh.assertTrue( obj.citation.opt );
	},
	function testCitationTokensGetsCreated (){
		var sys = new RhinoTest();
		var builder = new CSL.Core.Build(nestedsingleton);
		var obj = builder.build(sys);
		doh.assertTrue( obj.citation.tokens );
	},
	function testBibliographyOptGetsCreated (){
		var sys = new RhinoTest();
		var builder = new CSL.Core.Build(nestedsingleton);
		var obj = builder.build(sys);
		doh.assertTrue( obj.bibliography.opt );
	},
	function testBibliographyTokensGetsCreated (){
		var sys = new RhinoTest();
		var builder = new CSL.Core.Build(nestedsingleton);
		var obj = builder.build(sys);
		doh.assertTrue( obj.bibliography.tokens );
	},


	function testSetXmlInstantiation (){
		var obj = new CSL.Core.Build(nestedsingleton);
		function proc(){}
		function testme (){
			try {
				var setxml = new obj._getNavi(obj.showXml(),proc,true);
				return "Success";
			} catch (e){
				return e;
			}
		}
		var res = testme();
		doh.assertEqual("Success", res);
	},

	function testRunnerInstantiation (){
		var obj = new CSL.Core.Build(nestedsingleton);
		var state = obj.state;
		function testme (){
			try {
				var build = new obj._builder(state);
				return "Success";
			} catch (e){
				return e;
			}
		}
		var res = testme();
		doh.assertEqual("Success", res);
	},

]);
