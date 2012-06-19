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
//This file is the command-line entry point for running the tests in
//Rhino

/*=====
dojo.tests = {
	// summary: D.O.H. Test files for Dojo unit testing.
};
=====*/

//
// XXXXX rhino specific
//
LoadModule('/home/bennett/src/jslibs/Linux_32_opt/jsio');
LoadModule('/home/bennett/src/jslibs/Linux_32_opt/jsstd');
LoadModule('/home/bennett/src/jslibs/Linux_32_opt/jsiconv');

var dec = new Iconv('UCS-2-INTERNAL','UTF-8',true,false);
var enc = new Iconv('UTF-8','UCS-2-INTERNAL',false,true);

var load = Exec;

var print = function(txt){
	Print( enc(txt)+'\n');
}

var readFile = function(filename){
    var file = new File(filename);
    file.Open( File.RDONLY );
    var ret = dec( file.Read() );
    file.Close();
    return ret;
}

load("./dojo-sm/dojo/dojo.js");
dojo.registerModulePath("dojo", "./dojo-sm/dojo");
dojo.registerModulePath("dojox", "./dojo-sm/dojox");
dojo.registerModulePath("std", "./tests/bundled");
dojo.registerModulePath("citeproc_js", "./tests/citeproc-js");
dojo.registerModulePath("csl", "./src");
dojo.registerModulePath("csl.output", "./src/output");
dojo.registerModulePath("doh", "./dojo-sm/util/doh");

load("./src/xmle4x.js");
load("./src/xmldom.js");
load("./src/load.js");

print("#####");
print("Testing with tracemonkey");
print("#####");

load("./src/testing_rhino.js");
load("./src/testing_stdrhino.js");

load("./tests/runners/run.js");
