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

CSL.Node.text = {
    build: function (state, target) {
        var variable, func, form, plural, id, num, number, formatter, firstoutput, specialdelimiter, label, myname, names, name, year, suffix, term, dp, len, pos, n, m, value, flag;
        CSL.Util.substituteStart.call(this, state, target);
        if (this.postponed_macro) {
            //func = function(state, Item) {
            //    print("XXX macro_name: "+this.postponed_macro);
            //}
            //this.execs.push(func);
            CSL.expandMacro.call(state, this);
        } else {
            // ...
            //
            // Do non-macro stuff
            
            // Guess again. this.variables is ephemeral, adjusted by an initial
            // function set on the node via @variable attribute setup.
            //variable = this.variables[0];
            
            if (!this.variables_real) {
                this.variables_real = [];
            }
            if (!this.variables) {
                this.variables = [];
            }

            form = "long";
            plural = 0;
            if (this.strings.form) {
                form = this.strings.form;
            }
            if (this.strings.plural) {
                plural = this.strings.plural;
            }
            if ("citation-number" === this.variables_real[0] || "year-suffix" === this.variables_real[0] || "citation-label" === this.variables_real[0]) {
                //
                // citation-number and year-suffix are super special,
                // because they are rangeables, and require a completely
                // different set of formatting parameters on the output
                // queue.
                if (this.variables_real[0] === "citation-number") {
                    if (state.build.root === "citation") {
                        state.opt.update_mode = CSL.NUMERIC;
                    }
                    if (state.build.root === "bibliography") {
                        state.opt.bib_mode = CSL.NUMERIC;
                    }
                    if (state.build.area === "bibliography_sort") {
                        state.opt.citation_number_sort_used = true;
                    }
                    //this.strings.is_rangeable = true;
                    if ("citation-number" === state[state.tmp.area].opt.collapse) {
                        this.range_prefix = state.getTerm("range-delimiter");
                    }
                    this.successor_prefix = state[state.build.area].opt.layout_delimiter;
                    this.splice_prefix = state[state.build.area].opt.layout_delimiter;
                    func = function (state, Item, item) {
                        id = "" + Item.id;
                        if (!state.tmp.just_looking) {
                            if (item && item["author-only"]) {
                                state.tmp.element_trace.replace("do-not-suppress-me");
                                term = CSL.Output.Formatters["capitalize-first"](state, state.getTerm("reference", "long", "singular"));
                                state.output.append(term + " ");
                                state.tmp.last_element_trace = true;
                            }
                            if (item && item["suppress-author"]) {
                                if (state.tmp.last_element_trace) {
                                    state.tmp.element_trace.replace("suppress-me");
                                }
                                state.tmp.last_element_trace = false;
                            }
                            num = state.registry.registry[id].seq;
                            if (state.opt.citation_number_slug) {
                                state.output.append(state.opt.citation_number_slug, this);
                            } else {
                                number = new CSL.NumericBlob(num, this);
                                state.output.append(number, "literal");
                            }
                        }
                    };
                    this.execs.push(func);
                } else if (this.variables_real[0] === "year-suffix") {

                    state.opt.has_year_suffix = true;

                    if (state[state.tmp.area].opt.collapse === "year-suffix-ranged") {
                        //this.range_prefix = "-";
                        this.range_prefix = state.getTerm("range-delimiter");
                    }
                    this.successor_prefix = state[state.build.area].opt.layout_delimiter;
                    if (state[state.tmp.area].opt["year-suffix-delimiter"]) {
                        this.successor_prefix = state[state.build.area].opt["year-suffix-delimiter"];
                    }
                    func = function (state, Item) {
                        if (state.registry.registry[Item.id] && state.registry.registry[Item.id].disambig.year_suffix !== false && !state.tmp.just_looking) {
                            //state.output.append(state.registry.registry[Item.id].disambig[2],this);
                            num = parseInt(state.registry.registry[Item.id].disambig.year_suffix, 10);
                            number = new CSL.NumericBlob(num, this);
                            formatter = new CSL.Util.Suffixator(CSL.SUFFIX_CHARS);
                            number.setFormatter(formatter);
                            state.output.append(number, "literal");
                            //
                            // don't ask :)
                            // obviously the variable naming scheme needs
                            // a little touching up
                            firstoutput = false;
                            len = state.tmp.group_context.mystack.length;
                            for (pos = 0; pos < len; pos += 1) {
                                flag = state.tmp.group_context.mystack[pos];
                                if (!flag[2] && (flag[1] || (!flag[1] && !flag[0]))) {
                                    firstoutput = true;
                                    break;
                                }
                            }
                            // firstoutput = state.tmp.group_context.mystack.indexOf(true) === -1;
                            specialdelimiter = state[state.tmp.area].opt["year-suffix-delimiter"];
                            if (firstoutput && specialdelimiter && !state.tmp.sort_key_flag) {
                                state.tmp.splice_delimiter = state[state.tmp.area].opt["year-suffix-delimiter"];
                            }
                        }
                    };
                    this.execs.push(func);
                } else if (this.variables_real[0] === "citation-label") {
                    state.opt.has_year_suffix = true;
                    func = function (state, Item) {
                        label = Item["citation-label"];
                        if (!label) {
                            label = state.getCitationLabel(Item);
                        }
                        suffix = "";
                        if (state.registry.registry[Item.id] && state.registry.registry[Item.id].disambig.year_suffix !== false) {
                            num = parseInt(state.registry.registry[Item.id].disambig.year_suffix, 10);
                            suffix = state.fun.suffixator.format(num);
                        }
                        label += suffix;
                        state.output.append(label, this);
                    };
                    this.execs.push(func);
                }
            } else {
                if (this.strings.term) {
                    
                    // printterm
                    func = function (state, Item, item) {
                        var gender = state.opt.gender[Item.type];
                        var term = this.strings.term;
                        term = state.getTerm(term, form, plural, gender);
                        var myterm;
                        // if the term is not an empty string, say
                        // that we rendered a term
                        if (term !== "") {
                            flag = state.tmp.group_context.value();
                            //print("setting TERM to true [0]");
                            flag[0] = true;
                            state.tmp.group_context.replace(flag);
                        }
                        
                        // capitalize the first letter of a term, if it is the
                        // first thing rendered in a citation (or if it is
                        // being rendered immediately after terminal punctuation,
                        // I guess, actually).
                        if (!state.tmp.term_predecessor) {
                            myterm = CSL.Output.Formatters["capitalize-first"](state, term);
                            //CSL.debug("Capitalize");
                        } else {
                            if (item && item.prefix) {
                                var prefix = item.prefix.replace(/\s+$/, "");
                                if (CSL.TERMINAL_PUNCTUATION.slice(0,-1).indexOf(prefix.slice(-1)) > -1) {
                                    myterm = CSL.Output.Formatters["capitalize-first"](state, term);
                                } else {
                                    myterm = term;
                                }
                            } else {
                                myterm = term;
                            }
                        }
                        
                        // XXXXX Cut-and-paste code in multiple locations. This code block should be
                        // collected in a function.
                        // Tag: strip-periods-block
                        if (state.tmp.strip_periods) {
                            myterm = myterm.replace(/\./g, "");
                        } else {
                            for (var i = 0, ilen = this.decorations.length; i < ilen; i += 1) {
                                if ("@strip-periods" === this.decorations[i][0] && "true" === this.decorations[i][1]) {
                                    myterm = myterm.replace(/\./g, "");
                                    break;
                                }
                            }
                        }
                        state.output.append(myterm, this);
                    };
                    this.execs.push(func);
                    state.build.term = false;
                    state.build.form = false;
                    state.build.plural = false;
                } else if (this.variables_real.length) {
                    func = function (state, Item) {
                        var parallel_variable = this.variables[0];
                        if (parallel_variable === "title" && form === "short") {
                            parallel_variable = "shortTitle";
                        }
                        state.parallel.StartVariable(parallel_variable);
                        state.parallel.AppendToVariable(Item[parallel_variable]);
                    };
                    this.execs.push(func);

                    // plain string fields

                    // Deal with multi-fields and ordinary fields separately.
                    if (CSL.MULTI_FIELDS.indexOf(this.variables_real[0]) > -1) {
                        // multi-fields
                        // Initialize transform factory according to whether
                        // abbreviation is desired.
                        if (form === "short") {
                            state.transform.init(this, this.variables_real[0], this.variables_real[0]);
                            if (this.variables_real[0] === "container-title") {
                                state.transform.setAlternativeVariableName("journalAbbreviation");
                            } else if (this.variables_real[0] === "title") {
                                state.transform.setAlternativeVariableName("shortTitle");
                            }
                        } else {
                            state.transform.init(this, this.variables_real[0]);
                        }
                        if (state.build.extension) {
                            // multi-fields for sorting get a sort transform,
                            // (abbreviated if the short form was selected)
                            state.transform.init(this, this.variables_real[0], this.variables_real[0]);
                            state.transform.setTransformFallback(true);
                            func = state.transform.getOutputFunction(this.variables);
                        } else {
                            state.transform.setTransformFallback(true);
                            state.transform.setAbbreviationFallback(true);
                            func = state.transform.getOutputFunction(this.variables);
						}
                        if (this.variables_real[0] === "container-title") {
                            var xfunc = function (state, Item, item) {
                                if (Item['container-title'] && state.tmp.citeblob.has_volume) {
                                    state.tmp.citeblob.can_suppress_identical_year = true;
                                }
                            };
                            this.execs.push(xfunc);
                        }
                    } else {
                        // ordinary fields
                        if (CSL.CITE_FIELDS.indexOf(this.variables_real[0]) > -1) {
                            // per-cite fields are read from item, rather than Item
                            func = function (state, Item, item) {
                                if (item && item[this.variables[0]]) {
                                    var locator = "" + item[this.variables[0]];
                                    locator = locator.replace(/--*/g,"\u2013");
                                    state.output.append(locator, this);
                                }
                            };
                        } else if (this.variables_real[0] === "page-first") {
                            // page-first is a virtual field, consisting
                            // of the front slice of page.
                            func = function (state, Item) {
                                var idx, value;
                                value = state.getVariable(Item, "page", form);
                                if (value) {
                                    value = value.replace("\u2013", "-", "g");
                                    idx = value.indexOf("-");
                                    if (idx > -1) {
                                        value = value.slice(0, idx);
                                    }
                                    state.output.append(value, this);
                                }
                            };
                        } else  if (this.variables_real[0] === "page") {
                            // page gets mangled with the correct collapsing
                            // algorithm
                            func = function (state, Item) {
                                var value = state.getVariable(Item, "page", form);
                                if (value) {
                                    value = state.fun.page_mangler(value);
                                    state.output.append(value, this);
                                }
                            };
                        } else if (this.variables_real[0] === "volume") {
                            func = function (state, Item) {
                                if (this.variables[0]) {
                                    var value = state.getVariable(Item, this.variables[0], form);
                                    if (value) {
                                        // Only allow the suppression of a year identical
                                        // to the volume number if the container-title
                                        // is rendered after the volume number.
                                        state.tmp.citeblob.has_volume = true;
                                        state.output.append(value, this);
                                    }
                                }
                            };
                        } else if (this.variables_real[0] === "hereinafter") {
                            func = function (state, Item) {
                                var hereinafter_key = state.transform.getHereinafter(Item);
                                var value = state.transform.abbrevs["default"].hereinafter[hereinafter_key];
                                if (value) {
                                    state.tmp.group_context.value()[2] = true;
                                    state.output.append(value, this);
                                }
                            };
                        } else {
                            // anything left over just gets output in the normal way.
                            func = function (state, Item) {
                                var value;
                                if (this.variables[0]) {
                                    value = state.getVariable(Item, this.variables[0], form);
                                    if (value) {
                                        state.output.append(value, this);
                                    }
                                }
                            };
                        }
                    }
                    this.execs.push(func);
                    func = function (state, Item) {
                        state.parallel.CloseVariable("text");
                    };
                    this.execs.push(func);
                } else if (this.strings.value) {
                    // for the text value attribute.
                    func = function (state, Item) {
                        var flag;
                        flag = state.tmp.group_context.value();
                        // say that we rendered a term
                        //print("  setting [0] to true based on: " + this.strings.value);
                        flag[0] = true;
                        state.tmp.group_context.replace(flag);
                        state.output.append(this.strings.value, this);
                    };
                    this.execs.push(func);
                    // otherwise no output
                }
            }
            target.push(this);
        }
        CSL.Util.substituteEnd.call(this, state, target);
    }
};


