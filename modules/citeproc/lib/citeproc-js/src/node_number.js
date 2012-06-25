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
            var i, ilen, newlst, lst;
            // NOTE: this works because this is the ONLY function in this node.
            // If further functions are added, they need to start with the same
            // abort condition.
            if (this.variables.length === 0) {
                return;
            }
            var varname, num, number, m, j, jlen;
            varname = this.variables[0];
            state.parallel.StartVariable(this.variables[0]);
            if (this.variables[0] === "locator") {
                state.parallel.AppendToVariable(Item.section);
            } else {
                state.parallel.AppendToVariable(Item[this.variables[0]]);
            }

            var rex = new RegExp("(?:&|, | and |" + state.getTerm("page-range-delimiter") + ")");

            if (varname === 'collection-number' && Item.type === 'legal_case') {
                state.tmp.renders_collection_number = true;
            }
            
            // Only allow the suppression of a year identical
            // to collection-number if the container-title
            // is rendered after collection-number.
            var value = Item[this.variables[0]];
                       
            var form = "long";
            if (this.strings.label_form_override) {
                form = this.strings.label_form_override;
            }

            if (this.text_case_normal) {
                if (value) {
                    value = value.replace("\\", "");
                    state.output.append(value, this);
                }
            } else if (varname === "locator"
                       && item.locator) {
                
                // For bill or legislation items that have a label-form
                // attribute set on the cs:number node rendering the locator,
                // the form and pluralism of locator terms are controlled
                // separately from those of the initial label. Form is
                // straightforward: the label uses the value set on
                // the cs:label node that renders it, and the embedded
                // labels use the value of label-form set on the cs:number
                // node. Both default to "long".
                //
                // Pluralism is more complicated. For embedded labels,
                // pluralism is evaluated using a simple heuristic that
                // can be found below (it just looks for comma, ampersand etc).
                // The item.label rendered independently via cs:label
                // defaults to singular. It is always singular if embedded
                // labels exist that (when expanded to their valid CSL
                // value) do not match the value of item.label. Otherwise,
                // if one or more matching embedded labels exist, the
                // cs:label is set to plural.
                //
                // The code that does all this is divided between this module,
                // util_static_locator.js, and util_label.js. It's not easy
                // to follow, but seems to do the job. Let's home for good
                // luck out there in the wild.
                
                // Do replacements here
                item.locator = item.locator.replace(/([^\\])\s*-\s*/, "$1" + state.getTerm("page-range-delimiter"));
                // (actually, if numeric parsing is happening as it ought to, this won't be necessary)
                // or ... maybe note. We need to account for a missing page-range-format attribute.

                m = item.locator.match(CSL.STATUTE_SUBDIV_GROUPED_REGEX);
                if (m) {
                    lst = item.locator.split(CSL.STATUTE_SUBDIV_PLAIN_REGEX);
                    for (i = 0, ilen = lst.length; i < ilen; i += 1) {
                        lst[i] = state.fun.page_mangler(lst[i]);
                    }
                    newlst = [lst[0]];
                    
                    // Get form
                    if (!this.strings.label_form_override && state.tmp.group_context.value()[5]) {
                        form = state.tmp.group_context.value()[5];
                    }
                    for (i = 1, ilen = lst.length; i < ilen; i += 1) {
                        // For leading label: it is always singular if we are specifying subdivisions
                        // Rough guess at pluralism
                        var subplural = 0;
                        
                        if (lst[i].match(rex)) {
                            subplural = 1;
                        }
                        var term = CSL.STATUTE_SUBDIV_STRINGS[m[i - 1].replace(/^\s*/,"")];
                        var myform = form;
                        if (item.section_label_count > i && item.section_form_override) {
                            myform = item.section_form_override;
                        }
                        newlst.push(state.getTerm(term, myform, subplural));
                        newlst.push(lst[i].replace(/^\s*/,""));
                    }
                    value = newlst.join(" ");
                    value = value.replace(/\\/, "", "g");
                    state.output.append(value, this);
                } else {
                    value = state.fun.page_mangler(item.locator);
                    value = value.replace(/\\/, "", "g");
                    state.output.append(value, this);
                }
            } else {
                var node = this;
                if (!state.tmp.shadow_numbers[varname] 
                    || (state.tmp.shadow_numbers[varname].values.length 
                        && state.tmp.shadow_numbers[varname].values[0][2] === false)) {
                    if (varname === "locator") {
                        state.processNumber(node, item, varname, Item.type);
                    } else {
                        state.processNumber(node, Item, varname, Item.type);
                    }
                }
                var values = state.tmp.shadow_numbers[varname].values;
                var blob;
                // If prefix and suffix are nil, run through the page mangler,
                // if any. Otherwise, apply styling.
                var newstr = "";
                var rangeType = "page";
                if (["bill","gazette","legislation","legal_case","treaty"].indexOf(Item.type) > -1
                    && varname === "collection-number") {

                    rangeType = "year";
                }
                if (((varname === "number" 
                      && ["bill","gazette","legislation","treaty"].indexOf(Item.type) > -1)
                     || state.opt[rangeType + "-range-format"]) 
                    && !this.strings.prefix && !this.strings.suffix
                    && !this.strings.form) {
                    for (i = 0, ilen = values.length; i < ilen; i += 1) {
                        newstr += values[i][1];
                    }
                }
                if (newstr && !newstr.match(/^[\-.\u20130-9]+$/)) {
                    if (varname === "number" 
                        && ["bill","gazette","legislation","treaty"].indexOf(Item.type) > -1) {
                        
                        var firstword = newstr.split(/\s/)[0];
                        if (firstword) {
                            newlst = [];
                            m = newstr.match(CSL.STATUTE_SUBDIV_GROUPED_REGEX);
                            if (m) {
                                lst = newstr.split(CSL.STATUTE_SUBDIV_PLAIN_REGEX);
                                for (i = 1, ilen = lst.length; i < ilen; i += 1) {
                                    newlst.push(state.getTerm(CSL.STATUTE_SUBDIV_STRINGS[m[i - 1].replace(/^\s+/, "")], this.strings.label_form_override));
                                    newlst.push(lst[i].replace(/^\s+/, ""));
                                }
                                newstr = newlst.join(" ");
                            }
                        }
                    }
                    state.output.append(newstr, this);
                } else {
                    if (values.length) {
                        state.output.openLevel("empty");
                        for (i = 0, ilen = values.length; i < ilen; i += 1) {
                            blob = new CSL[values[i][0]](values[i][1], values[i][2], Item.id);
                            if (i > 0) {
                                blob.strings.prefix = blob.strings.prefix.replace(/^\s*/, "");
                            }
                            if (i < values.length - 1) {
                                blob.strings.suffix = blob.strings.suffix.replace(/\s*$/, "");
                            }
                            state.output.append(blob, "literal", false, false, true);
                        }
                        state.output.closeLevel("empty");
                    }
                }
            }
            if (varname === "locator") {
                // Only render the locator variable once in a cite.
                state.tmp.done_vars.push("locator");
            }
            state.parallel.CloseVariable("number");
        };
        this.execs.push(func);

        target.push(this);
        CSL.Util.substituteEnd.call(this, state, target);
    }
};


