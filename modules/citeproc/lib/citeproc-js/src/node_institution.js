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

/*global CSL: true */

CSL.Node.institution = {
    build: function (state, target) {
        if ([CSL.SINGLETON, CSL.START].indexOf(this.tokentype) > -1) {

            if ("string" === typeof state.build.name_delimiter && !this.strings.delimiter) {
                this.strings.delimiter = state.build.name_delimiter;
            }

            var myand, and_default_prefix, and_suffix;
            // This is the same code for the same result as in node_name.js, 
            // but when cs:institution comes on stream, it may produce
            // different results.
            if ("text" === this.strings.and) {
                this.and_term = state.getTerm("and", "long", 0);
            } else if ("symbol" === this.strings.and) {
                this.and_term = "&";
            }
            if ("undefined" === typeof this.and_term && state.build.and_term) {
                this.and_term = state.getTerm("and", "long", 0);
            }
            if (CSL.STARTSWITH_ROMANESQUE_REGEXP.test(this.and_term)) {
                this.and_prefix_single = " ";
                this.and_prefix_multiple = ", ";
                if ("string" === typeof this.strings.delimiter) {
                    this.and_prefix_multiple = this.strings.delimiter;
                }
                this.and_suffix = " ";
            } else {
                this.and_prefix_single = "";
                this.and_prefix_multiple = "";
                this.and_suffix = "";
            }
            if (this.strings["delimiter-precedes-last"] === "always") {
                this.and_prefix_single = this.strings.delimiter;
            } else if (this.strings["delimiter-precedes-last"] === "never") {
                // Slightly fragile: could test for charset here to make
                // this more certain.
                if (this.and_prefix_multiple) {
                    this.and_prefix_multiple = " ";
                }
            }
            
            func = function (state, Item) {
                this.and = {};
                if ("undefined" !== typeof this.and_term) {
                    state.output.append(this.and_term, "empty", true);
                    this.and.single = state.output.pop();
                    this.and.single.strings.prefix = this.and_prefix_single;
                    this.and.single.strings.suffix = this.and_suffix;
                    state.output.append(this.and_term, "empty", true);
                    this.and.multiple = state.output.pop();
                    this.and.multiple.strings.prefix = this.and_prefix_multiple;
                    this.and.multiple.strings.suffix = this.and_suffix;
                } else if ("undefined" !== this.strings.delimiter) {
                    this.and.single = new CSL.Blob(this.strings.delimiter);
                    this.and.single.strings.prefix = "";
                    this.and.single.strings.suffix = "";
                    this.and.multiple = new CSL.Blob(this.strings.delimiter);
                    this.and.multiple.strings.prefix = "";
                    this.and.multiple.strings.suffix = "";
                }
                
                /*
                if (this.strings["delimiter-precedes-last"] === "always") {
                    this.and.single.strings.prefix = this.strings.delimiter;
                    this.and.multiple.strings.prefix = this.strings.delimiter;
                } else if (this.strings["delimiter-precedes-last"] === "contextual") {
                    this.and.single.strings.prefix = and_default_prefix;
                    this.and.multiple.strings.prefix = this.strings.delimiter;
                } else {
                    this.and.single.strings.prefix = and_default_prefix;
                    this.and.multiple.strings.prefix = and_default_prefix;
                }
                */
                state.nameOutput.institution = this;
            };
            this.execs.push(func);
        }
        target.push(this);
    },
    configure: function (state, pos) {
        if ([CSL.SINGLETON, CSL.START].indexOf(this.tokentype) > -1) {
            state.build.has_institution = true;
        }
    }
};
