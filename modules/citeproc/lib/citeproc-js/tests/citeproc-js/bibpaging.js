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

dojo.provide("citeproc_js.bibpaging");

var mycsl = "<style><citation><layout><text value=\"ignoreme\"/></layout></citation><bibliography><layout><text variable=\"title\"/></layout></bibliography></style>";

var myitems = [
    {
        "id":"ITEM-1",
        "type": "book",
        "title": "Item One"
    },
    {
        "id":"ITEM-2",
        "type": "book",
        "title": "Item Two"
    },
    {
        "id":"ITEM-3",
        "type": "book",
        "title": "Item Three"
    },
    {
        "id":"ITEM-4",
        "type": "book",
        "title": "Item Four"
    }
];

doh.register("citeproc_js.bibpaging", [

	function testOne() {
		function testme () {
			try {
			    var mysys = new RhinoTest();
                var myids = [];
                for (var i = 0, ilen = myitems.length; i < ilen; i += 1) {
                    mysys._cache[myitems[i].id] = myitems[i];
                    myids.push(myitems[i].id);
                }
				var citeproc = new CSL.Engine(mysys,mycsl);
                citeproc.updateItems(myids);
                var myparams = {page_start:true,page_length:1};
                var ret = citeproc.makeBibliography(myparams);
				return ret;
			} catch (e) {
				return e;
			}
		}
		var res = testme();
		doh.assertEqual( false, res[0].done);
		doh.assertEqual( res[1][0], "  <div class=\"csl-entry\">Item One</div>\n" );
	},

	function testTwo() {
		function testme () {
			try {
			    var mysys = new RhinoTest();
                var myids = [];
                for (var i = 0, ilen = myitems.length; i < ilen; i += 1) {
                    mysys._cache[myitems[i].id] = myitems[i];
                    myids.push(myitems[i].id);
                }
				var citeproc = new CSL.Engine(mysys,mycsl);
                citeproc.updateItems(myids);
                var myparams = {page_start:true,page_length:20};
                var ret = citeproc.makeBibliography(myparams);
				return ret;
			} catch (e) {
				return e;
			}
		}
		var res = testme();
		doh.assertEqual( true, res[0].done);
	},

	function testThree() {
		function testme () {
			try {
			    var mysys = new RhinoTest();
                var myids = [];
                for (var i = 0, ilen = myitems.length; i < ilen; i += 1) {
                    mysys._cache[myitems[i].id] = myitems[i];
                    myids.push(myitems[i].id);
                }
				var citeproc = new CSL.Engine(mysys,mycsl);
                citeproc.updateItems(myids);
                var myparams = {page_start:"ITEM-1",page_length:2};
                var ret = citeproc.makeBibliography(myparams);
				return ret;
			} catch (e) {
				return e;
			}
		}
		var res = testme();
		doh.assertEqual( false, res[0].done);
        doh.assertEqual( 2, res[1].length);
        doh.assertEqual( "  <div class=\"csl-entry\">Item Two</div>\n", res[1][0]);
        doh.assertEqual( "  <div class=\"csl-entry\">Item Three</div>\n", res[1][1]);
	}
]);

