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

dojo.provide("citeproc_js.flipflopper");

doh.register("citeproc_js.flipflopper", [
	function testProcessTagsUnopenedCloseTags(){
		var myxml = "<style></style>";
		var sys = new RhinoTest();
		var state = new CSL.Engine(sys,myxml);
		var ff = new CSL.Util.FlipFlopper(state);
		ff.init("head<i>italic</b>bold's");
		ff.processTags();
		doh.assertEqual(39,ff.blob.blobs.length);
		doh.assertEqual("head&#60;i&#62;italic&#60;/b&#62;bold\u2019s",ff.blob.blobs);
	},
	function testImmediateClosingSingleQuote(){
		var myxml = "<style></style>";
		var sys = new RhinoTest();
		var state = new CSL.Engine(sys,myxml);
		var ff = new CSL.Util.FlipFlopper(state);
		ff.init("O\u2019Malley");
		ff.processTags();
		doh.assertEqual(8,ff.blob.blobs.length);
		doh.assertEqual("O\u2019Malley",ff.blob.blobs);
	},
	function testGetSplitStringsTwo(){
		var myxml = "<style></style>";
		var sys = new RhinoTest();
		var state = new CSL.Engine(sys,myxml);
		var ff = new CSL.Util.FlipFlopper(state);
		ff.init("hello\\<b>hello\\</b>again<i>ok</i>now");
		doh.assertEqual(5, ff.strs.length);
		doh.assertEqual("hello&#60;b&#62;hello&#60;/b&#62;again", ff.strs[0]);
		doh.assertEqual("ok", ff.strs[2]);
		doh.assertEqual("</i>",ff.strs[3]);
	},
	function testGetSplitStringsOne(){
		var myxml = "<style></style>";
		var sys = new RhinoTest();
		var state = new CSL.Engine(sys,myxml);
		var ff = new CSL.Util.FlipFlopper(state);
		ff.init("hello\\<b>hello");
		doh.assertEqual(1, ff.strs.length);
		doh.assertEqual(21, ff.strs[0].length);
		doh.assertEqual("hello&#60;b&#62;hello",ff.strs[0]);
	},
	function testProcessTagsOpenEnded(){
		var myxml = "<style></style>";
		var sys = new RhinoTest();
		var state = new CSL.Engine(sys,myxml);
		var ff = new CSL.Util.FlipFlopper(state);
		ff.init("hello <i>italic <b>bold+italic</b> YY </i>ITALIC \"quote -- <i>important");
		ff.processTags();
		doh.assertEqual(3,ff.blob.blobs.length);
		doh.assertEqual("hello ",ff.blob.blobs[0].blobs);
		doh.assertEqual("italic ",ff.blob.blobs[1].blobs[0].blobs);
		doh.assertEqual("bold+italic",ff.blob.blobs[1].blobs[1].blobs[0].blobs);
		doh.assertEqual(" YY ",ff.blob.blobs[1].blobs[2].blobs);
		doh.assertEqual("ITALIC \u201cquote -- &#60;i&#62;important",ff.blob.blobs[2].blobs);
	},
	function testProcessTagsCrossNesting(){
		var myxml = "<style></style>";
		var sys = new RhinoTest();
		var state = new CSL.Engine(sys,myxml);
		var ff = new CSL.Util.FlipFlopper(state);
		ff.init("hello <i>italic <b>bold+italic</b> YY </i>italic \"quote <b>XXhello</b>ZZ");
		ff.processTags();
		doh.assertEqual(5,ff.blob.blobs.length);
		doh.assertEqual("italic \u201cquote ",ff.blob.blobs[2].blobs);
		doh.assertEqual("XXhello",ff.blob.blobs[3].blobs[0].blobs);
		doh.assertEqual("ZZ",ff.blob.blobs[4].blobs);

		//doh.assertEqual("ZZ",ff.blob.blobs[2].blobs);
		//doh.assertEqual("hello</b>",ff.blob.blobs[4].blobs);
		//
		// i.e. [<blob.blobs:"italic ">,<blob>,<blob.blobs:" YY ">]
		//
		//doh.assertEqual(3, ff.blob.blobs[1].blobs.length);
	},
]);


var x = [
]
