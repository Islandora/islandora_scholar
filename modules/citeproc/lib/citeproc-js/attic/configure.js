/*
 * Copyright (c) Frank G. Bennett, Jr. 2009. All Rights Reserved.
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
 * Copyright (c) Frank G. Bennett, Jr. 2009. All Rights Reserved.
 */
dojo.provide("csl.configure");
if (!CSL) {
	load("./src/csl.js");
}

// ABANDONED

/**
 * Second-stage compiler.
 * <p>Instantiates with the raw style object returned by
 * {@link CSL.Core.Build#build}, and	provides a method that
 * returns a copy of the object stripped of its build
 * and configure areas, and complete with token jump
 * points for conditional branching, output rendering
 * functions, and methods for processing data items.</p>
 * @namespace Style configuration
 * @param {Object} builderobject The object output
 * by {@link CSL.Core.Build#build}.
 * @param {String} mode Optional.  Default is "html".
 * @example
 * builder = CSL.Core.Build(myxml)
 * raw_engine = builder.build()
 * configurator = CSL.Core.Configure(raw_engine,"rtf")
 * style_engine = configurator.configure()
 */
CSL.Core.Configure = function(state,mode) {
	this.state = state;
	if (!mode){
	    mode = "html";
	}

	if (this.state.build){
		delete this.state.build;
	}
	this.state.fun.decorate = CSL.Factory.Mode(mode);
	this.state.opt.mode = mode;
};


/**
 * Configure the citation style.
 * <p>In a single back-to-front pass over the token list, this sets
 * jump positions on conditional tokens (<code>if</code>,
 * <code>else-if</code>, <code>else</code>), installs rendering
 * functions used to generate output,
 * deletes the <code>build</code> and
 * <code>configure</code> areas, and attaches the iterface methods
 * from {@link CSL.Engine} that are needed for processing data
 * items.</p>
 */
CSL.Core.Configure.prototype.configure = function(){
	for each (var area in ["citation", "citation_sort", "bibliography","bibliography_sort"]){
		for (var pos=(this.state[area].tokens.length-1); pos>-1; pos--){
			var token = this.state[area].tokens[pos];
			token["next"] = (pos+1);
			if (token.name && CSL.Lib.Elements[token.name].configure){
				CSL.Lib.Elements[token.name].configure.call(token,this.state,pos);
			}
		}
	}
	this.state["version"] = CSL.Factory.version;
	return this.state;
};
