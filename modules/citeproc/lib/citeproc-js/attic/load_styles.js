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

dojo.provide("citeproc_js.load_styles");


var tryStyle = function(style){
	try {
		var sty = readFile("style/"+style+".csl");
		if (!sty){
			throw "Did not find style file: style/"+style+".csl";
		}
		var sys = new RhinoTest();
		var res = new CSL.Engine(sys,sty);
	} catch(e) {
		CSL.debug("oops: "+e);
	}
	return res;
}

doh.register("citeproc_js.load_styles", [
	function(){
		var res = tryStyle("mhra");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("ama");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("apa");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("apsa");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("asa");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("chicago-author-date");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("chicago-fullnote-bibliography");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("chicago-note-bibliography");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("chicago-note");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("harvard1");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("ieee");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("mhra_note_without_bibliography");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("mla");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("nature");
		doh.assertTrue( res );
	},
	function(){
		var res = tryStyle("nlm");
		doh.assertTrue( res );
	},
]);
