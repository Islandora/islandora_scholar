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

CSL.Node.number = {
    build: function (state, target) {
        var func;
        CSL.Util.substituteStart.call(this, state, target);
        //
        // This should push a rangeable object to the queue.
        //
        if (this.strings.form === "roman") {
            this.formatter = state.fun.romanizer;
        } else if (this.strings.form === "ordinal") {
            this.formatter = state.fun.ordinalizer;
        } else if (this.strings.form === "long-ordinal") {
            this.formatter = state.fun.long_ordinalizer;
        }
        if ("undefined" === typeof this.successor_prefix) {
            this.successor_prefix = state[state.build.area].opt.layout_delimiter;
        }
        if ("undefined" === typeof this.splice_prefix) {
            this.splice_prefix = state[state.build.area].opt.layout_delimiter;
        }
        // is this needed?
        //if ("undefined" === typeof this.splice_prefix){
        //    this.splice_prefix = state[state.tmp.area].opt.layout_delimiter;
        //}
        //
        // Whether we actually stick a number object on
        // the output queue depends on whether the field
        // contains a pure number.
        //
        // push number or text
        func = function (state, Item, item) {
            var varname, num, number, m, j, jlen;
            varname = this.variables[0];
            state.parallel.StartVariable(this.variables[0]);
            state.parallel.AppendToVariable(Item[this.variables[0]]);

            if (varname === "page-range" || varname === "page-first") {
                varname = "page";
            }
            var node = this;
            if (!state.tmp.shadow_numbers[varname] 
                || (state.tmp.shadow_numbers[varname].values.length 
                    && state.tmp.shadow_numbers[varname].values[0][2] === false)) {
                if (varname === "locator") {
                    state.processNumber(node, item, varname);
                } else {
                    state.processNumber(node, Item, varname);
                }
            }
            var values = state.tmp.shadow_numbers[varname].values;
            var blob;
            // If prefix and suffix are nil, run through the page mangler,
            // if any. Otherwise, apply styling.
            if (state.opt["page-range-format"] 
                && !this.strings.prefix && !this.strings.suffix
                && !this.strings.form) {
                var newstr = ""
                for (var i = 0, ilen = values.length; i < ilen; i += 1) {
                    newstr += values[i][1];
                }
                newstr = state.fun.page_mangler(newstr);
                state.output.append(newstr, this);
            } else {
                state.output.openLevel("empty");
                for (var i = 0, ilen = values.length; i < ilen; i += 1) {
                    var blob = new CSL[values[i][0]](values[i][1], values[i][2]);
                    if (i > 0) {
                        blob.strings.prefix = blob.strings.prefix.replace(/^\s*/, "");
                    }
                    if (i < values.length - 1) {
                        blob.strings.suffix = blob.strings.suffix.replace(/\s*$/, "");
                    }
                    state.output.append(blob, "literal");
                }
                state.output.closeLevel("empty");
            }
            state.parallel.CloseVariable("number");
        };
        this.execs.push(func);

        target.push(this);
        CSL.Util.substituteEnd.call(this, state, target);
    }
};


