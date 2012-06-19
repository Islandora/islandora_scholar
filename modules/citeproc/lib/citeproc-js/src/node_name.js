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

CSL.Node.name = {
    build: function (state, target) {
        var func, pos, len, attrname;
        if ([CSL.SINGLETON, CSL.START].indexOf(this.tokentype) > -1) {
            //state.fixOpt(this, "names-delimiter", "delimiter");
            state.fixOpt(this, "name-delimiter", "name_delimiter");
            state.fixOpt(this, "name-form", "form");
            
            state.fixOpt(this, "and", "and");
            state.fixOpt(this, "delimiter-precedes-last", "delimiter-precedes-last");
            state.fixOpt(this, "delimiter-precedes-et-al", "delimiter-precedes-et-al");
            state.fixOpt(this, "initialize-with", "initialize-with");
            state.fixOpt(this, "initialize", "initialize");
            state.fixOpt(this, "name-as-sort-order", "name-as-sort-order");
            state.fixOpt(this, "sort-separator", "sort-separator");
            state.fixOpt(this, "and", "and");

            state.fixOpt(this, "et-al-min", "et-al-min");
            state.fixOpt(this, "et-al-use-first", "et-al-use-first");
            state.fixOpt(this, "et-al-use-last", "et-al-use-last");
            state.fixOpt(this, "et-al-subsequent-min", "et-al-subsequent-min");
            state.fixOpt(this, "et-al-subsequent-use-first", "et-al-subsequent-use-first");

            // Many CSL styles pointlessly set et-al-[min|use-first]
            // and et-al-subsequent-[min|use-first] to the same
            // value.
            // Set state.opt.update_mode = CSL.POSITION if
            // et-al-subsequent-min or et-al-subsequent-use-first
            // are set AND their value differs from their plain
            // counterparts.
            if (this.strings["et-al-subsequent-min"]
                && (this.strings["et-al-subsequent-min"] !== this.strings["et-al-min"])) {
                
                state.opt.update_mode = CSL.POSITION;
            }
            if (this.strings["et-al-subsequent-use-first"]
                && (this.strings["et-al-subsequent-use-first"] !== this.strings["et-al-use-first"])) {
                
                state.opt.update_mode = CSL.POSITION;
            }

            // Et-al (onward processing in node_etal.js and node_names.js)
            // XXXXX Why is this necessary? This is available on this.name, right?
            state.build.etal_term = "et-al";
            state.build.name_delimiter = this.strings.delimiter;
            state.build["delimiter-precedes-et-al"] = this.strings["delimiter-precedes-et-al"];
            
            // Fix default delimiter, in a way that allows explicit
            // empty strings.
            if ("undefined" == typeof this.strings.name_delimiter) {
                this.strings.delimiter = ", ";
            } else {
                this.strings.delimiter = this.strings.name_delimiter;
            }

            // And
            if ("text" === this.strings.and) {
                this.and_term = state.getTerm("and", "long", 0);
            } else if ("symbol" === this.strings.and) {
                this.and_term = "&";
            }

            state.build.and_term = this.and_term;

            if (CSL.STARTSWITH_ROMANESQUE_REGEXP.test(this.and_term)) {
                this.and_prefix_single = " ";
                this.and_prefix_multiple = ", ";
                // Workaround to allow explicit empty string
                // on cs:name delimiter.
                if ("string" === typeof this.strings.delimiter) {
                    this.and_prefix_multiple = this.strings.delimiter;
                }
                this.and_suffix = " ";

                state.build.name_delimiter = this.strings.delimiter;

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

            if (this.strings["et-al-use-last"]) {
                // We use the dedicated Unicode ellipsis character because
                // it is recommended by some editors, and can be more easily
                // identified for find and replace operations.
                // Source: http://en.wikipedia.org/wiki/Ellipsis#Computer_representations
                //
                // Eventually, this should be localized as a term in CSL, with some
                // mechanism for triggering appropriate punctuation handling around
                // the ellipsis placeholder (Polish is a particularly tough case for that).
                this.ellipsis_term = "\u2026";
                // Similar treatment to "and", above, will be needed
                // here when this becomes a locale term.
                this.ellipsis_prefix_single = " ";
                this.ellipsis_prefix_multiple =  this.strings.delimiter;
                this.ellipsis_suffix = " ";
            }

            func = function (state, Item) {
                this.and = {};
                if (this.strings.and) {
                    state.output.append(this.and_term, "empty", true);
                    this.and.single = state.output.pop();
                    this.and.single.strings.prefix = this.and_prefix_single;
                    this.and.single.strings.suffix = this.and_suffix;
                    state.output.append(this.and_term, "empty", true);
                    this.and.multiple = state.output.pop();
                    this.and.multiple.strings.prefix = this.and_prefix_multiple;
                    this.and.multiple.strings.suffix = this.and_suffix;
                } else if (this.strings.delimiter) {
                    // This is a little weird, but it works.
                    this.and.single = new CSL.Blob(this.strings.delimiter);
                    this.and.single.strings.prefix = "";
                    this.and.single.strings.suffix = "";
                    this.and.multiple = new CSL.Blob(this.strings.delimiter);
                    this.and.multiple.strings.prefix = "";
                    this.and.multiple.strings.suffix = "";
                }

                this.ellipsis = {};
                if (this.strings["et-al-use-last"]) {
                    this.ellipsis.single = new CSL.Blob(this.ellipsis_term);
                    this.ellipsis.single.strings.prefix = this.ellipsis_prefix_single;
                    this.ellipsis.single.strings.suffix = this.ellipsis_suffix;
                    this.ellipsis.multiple = new CSL.Blob(this.ellipsis_term);
                    this.ellipsis.multiple.strings.prefix = this.ellipsis_prefix_multiple;
                    this.ellipsis.multiple.strings.suffix = this.ellipsis_suffix;
                }

                // et-al parameters are annoyingly incomprehensible
                // again.
                //
                // Explanation probably just adds a further layer of
                // irritation, but what's INTENDED here is that
                // the state.tmp et-al variables are set from the
                // cs:key element when composing sort keys, and a
                // macro containing a name can be called from cs:key.
                // So when cs:key sets et-al attributes, they are
                // set on state.tmp, and when the key is finished
                // processing, the state.tmp variables are reset to
                // undefined. IN THEORY the state.tmp et-al variables
                // will not be used in other contexts. I hope.
                //
                // Anyway, the current tests now seem to pass.
                if ("undefined" === typeof state.tmp["et-al-min"]) {
                    state.tmp["et-al-min"] = this.strings["et-al-min"];
                }
                if ("undefined" === typeof state.tmp["et-al-use-first"]) {
                    state.tmp["et-al-use-first"] = this.strings["et-al-use-first"];
                }
                if ("undefined" === typeof state.tmp["et-al-use-last"]) {
                    //print("  setting et-al-use-last from name: "+this.strings["et-al-use-last"]);
                    state.tmp["et-al-use-last"] = this.strings["et-al-use-last"];
                }

                state.nameOutput.name = this;
            };
            
            state.build.name_flag = true;

            this.execs.push(func);
        }
        target.push(this);
    }
};


