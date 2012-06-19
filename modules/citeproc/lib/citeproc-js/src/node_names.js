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

CSL.Node.names = {
    build: function (state, target) {
        var func, len, pos, attrname;
        var debug = false;
        // CSL.debug = print;
        
        if (this.tokentype === CSL.START || this.tokentype === CSL.SINGLETON) {
            CSL.Util.substituteStart.call(this, state, target);
            state.build.substitute_level.push(1);
            
            state.fixOpt(this, "names-delimiter", "delimiter");

        }
        
        if (this.tokentype === CSL.SINGLETON) {
            func = function (state, Item, item) {
                state.nameOutput.reinit(this);
            };
            this.execs.push(func);
        }


        if (this.tokentype === CSL.START) {

            state.build.names_flag = true;

            // init can substitute
            // init names
            func = function (state, Item, item) {
                state.tmp.can_substitute.push(true);
                state.parallel.StartVariable("names");
                state.nameOutput.init(this);
            };
            this.execs.push(func);

        }
        
        if (this.tokentype === CSL.END) {

            for (var i = 0, ilen = 3; i < ilen; i += 1) {
                var key = ["family", "given", "et-al"][i];
                this[key] = state.build[key];
                state.build[key] = undefined;
            }

            // Labels, if any
            this.label = state.build.name_label;
            state.build.name_label = undefined;

            // The with term. This isn't the right place
            // for this, but it's all hard-wired at the
            // moment.
            var mywith = "with";

            var with_default_prefix = "";
            var with_suffix = "";
            if (CSL.STARTSWITH_ROMANESQUE_REGEXP.test(mywith)) {
                with_default_prefix = " ";
                with_suffix = " ";
            }
            this["with"] = {};
            this["with"].single = new CSL.Blob(mywith);
            this["with"].single.strings.suffix = with_suffix;
            this["with"].multiple = new CSL.Blob(mywith);
            this["with"].multiple.strings.suffix = with_suffix;
            if (this.strings["delimiter-precedes-last"] === "always") {
                this["with"].single.strings.prefix = this.strings.delimiter;
                this["with"].multiple.strings.prefix = this.strings.delimiter;
            } else if (this.strings["delimiter-precedes-last"] === "contextual") {
                this["with"].single.strings.prefix = with_default_prefix;
                this["with"].multiple.strings.prefix = this.strings.delimiter;
            } else {
                this["with"].single.strings.prefix = with_default_prefix;
                this["with"].multiple.strings.prefix = with_default_prefix;
            }

            // Et-al (strings only)
            // Blob production has to happen inside nameOutput()
            // since proper escaping requires access to the output
            // queue.
            if (state.build.etal_node) {
                this.etal_style = state.build.etal_node;
            } else {
                this.etal_style = "empty";
            }
            this.etal_term = state.getTerm(state.build.etal_term, "long", 0);
            if (CSL.STARTSWITH_ROMANESQUE_REGEXP.test(this.etal_term)) {
                this.etal_prefix_single = " ";
                // Should be name delimiter, not hard-wired.
                this.etal_prefix_multiple = state.build.name_delimiter;
                if (state.build["delimiter-precedes-et-al"] === "always") {
                    this.etal_prefix_single = state.build.name_delimiter;
                } else if (state.build["delimiter-precedes-et-al"] === "never") {
                    this.etal_prefix_multiple = " ";
                }
                this.etal_suffix = "";
            } else {
                this.etal_prefix_single = "";
                this.etal_prefix_multiple = "";
                this.etal_suffix = "";
            }
            // et-al affixes are further adjusted in nameOutput(),
            // after the term (possibly changed in cs:et-al) is known.
            

            // "and" and "ellipsis" are set in node_name.js
            func = function (state, Item, item) {
                for (var i = 0, ilen = 3; i < ilen; i += 1) {
                    var key = ["family", "given"][i];
                    state.nameOutput[key] = this[key];
                }
                state.nameOutput["with"] = this["with"];
                state.nameOutput.label = this.label;
                state.nameOutput.etal_style = this.etal_style;
                state.nameOutput.etal_term = this.etal_term;
                state.nameOutput.etal_prefix_single = this.etal_prefix_single;
                state.nameOutput.etal_prefix_multiple = this.etal_prefix_multiple;
                state.nameOutput.etal_suffix = this.etal_suffix;
                state.nameOutput.outputNames();
                state.tmp["et-al-use-first"] = undefined;
                state.tmp["et-al-min"] = undefined;
                state.tmp["et-al-use-last"] = undefined;
            };
            this.execs.push(func);

            // unsets
            func = function (state, Item) {
                if (!state.tmp.can_substitute.pop()) {
                    state.tmp.can_substitute.replace(false, CSL.LITERAL);
                }
                
                state.parallel.CloseVariable("names");

                state.tmp.can_block_substitute = false;
            };
            this.execs.push(func);

            state.build.name_flag = false;
        }
        target.push(this);

        if (this.tokentype === CSL.END || this.tokentype === CSL.SINGLETON) {
            state.build.substitute_level.pop();
            CSL.Util.substituteEnd.call(this, state, target);
        }
    }
};
