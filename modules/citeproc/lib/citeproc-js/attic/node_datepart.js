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

CSL.Node["date-part"] = {
	build: function (state, target) {
		var func, pos, len, decor;
		if (!this.strings.form) {
			this.strings.form = "long";
		}
		if (state.build.datexml) {
			len = this.decorations.length;
			print("HELLO");
			for each (var decor in this.decorations) {
				//
				// Xml: find one node by attribute value and set attribute value
				//
				state.sys.xml.setAttributeOnNodeIdentifiedByNameAttribute(state.build.datexml, 'date-part', this.strings.name, decor[0], decor[1]);
			}
			for (var attr in this.strings) {
				if (attr === "name" || attr === "prefix" || attr === "suffix") {
					continue;
				}
				//
				// Xml: find one node by attribute value and set attribute value
				//
				state.sys.xml.setAttributeOnNodeIdentifiedByNameAttribute(state.build.datexml, 'date-part', this.strings.name, attr, this.strings[attr]);
			}
		} else {
			// used in node_date, to send a list of rendering date parts
			// to node_key, for dates embedded in macros.
			state.build.date_parts.push(this.strings.name);
			//
			// Set delimiter here, if poss.
			//
			var render_date_part = function (state, Item) {
				var first_date = true;
				var value = "";
				var value_end = "";
				state.tmp.donesies.push(this.strings.name);
				if (state.tmp.date_object) {
					value = state.tmp.date_object[this.strings.name];
					value_end = state.tmp.date_object[(this.strings.name+"_end")];
					//if ("undefined" === typeof value_end && state.tmp.area.slice(-5) === "_sort"){
					//	print("sorting it");
					//}
				}
				if ("year" === this.strings.name && value === 0 && !state.tmp.suppress_decorations) {
					value = state.getTerm("no date");
				}
				var real = !state.tmp.suppress_decorations;
				var have_collapsed = state.tmp.have_collapsed;
				var invoked = state[state.tmp.area].opt.collapse === "year-suffix" || state[state.tmp.area].opt.collapse === "year-suffix-ranged";
				var precondition = state[state.tmp.area].opt["disambiguate-add-year-suffix"];
				if (real && precondition && invoked) {
					state.tmp.years_used.push(value);
					var known_year = state.tmp.last_years_used.length >= state.tmp.years_used.length;
					if (known_year && have_collapsed) {
						if (state.tmp.last_years_used[(state.tmp.years_used.length-1)] === value) {
							value = false;
						}
					}
				}
				if ("undefined" != typeof value) {
					var bc = false;
					var ad = false;
					var bc_end = false;
					var ad_end = false;
					if ("year" === this.strings.name) {
						if (parseInt(value, 10) < 500 && parseInt(value, 10) > 0) {
							ad = state.getTerm("ad");
						}
						if (parseInt(value, 10) < 0) {
							bc = state.getTerm("bc");
							value = (parseInt(value, 10) * -1);
						}
						if (value_end) {
							if (parseInt(value_end, 10) < 500 && parseInt(value_end, 10) > 0) {
								ad_end = state.getTerm("ad");
							}
							if (parseInt(value_end, 10) < 0) {
								bc_end = state.getTerm("bc");
								value_end = (parseInt(value_end, 10) * -1);
							}
						}
					}

					state.parallel.AppendToVariable(value);

					if (this.strings.form) {
						value = CSL.Util.Dates[this.strings.name][this.strings.form](state, value);
						if (value_end) {
							value_end = CSL.Util.Dates[this.strings.name][this.strings.form](state, value_end);
						}
					}
					state.output.openLevel("empty");
					if (state.tmp.date_collapse_at.length) {
						//state.output.startTag(this.strings.name,this);
						var ready = true;
						for each (var item in state.tmp.date_collapse_at) {
							if (state.tmp.donesies.indexOf(item) === -1) {
								ready = false;
								break;
							}
						}
						if (ready) {
							if (value_end != "0") {
								if (state.dateput.queue.length === 0) {
									first_date = true;
								}
								state.dateput.append(value_end, this);
								if (first_date) {
									state.dateput.current.value()[0].strings.prefix = "";
								}
							}
							state.output.append(value, this);
							var curr = state.output.current.value();
							curr.blobs[(curr.blobs.length-1)].strings.suffix="";
							state.output.append(this.strings["range-delimiter"], "empty");
							var dcurr = state.dateput.current.value();
							// if (dcurr.length){
							// 	print("prefix: "+dcurr[0].blobs[0].strings.prefix);
							// }
							curr.blobs = curr.blobs.concat(dcurr);
							state.dateput.string(state, state.dateput.queue);
							state.tmp.date_collapse_at = [];
						} else {
							state.output.append(value, this);
							// print("collapse_at: "+state.tmp.date_collapse_at);
							if (state.tmp.date_collapse_at.indexOf(this.strings.name) > -1) {
								//
								// Use ghost dateput queue
								//
								if (value_end != "0") {
									//
									// XXXXX: It's a workaround.  It's ugly.
									// There's another one above.
									//
									if (state.dateput.queue.length === 0) {
										first_date = true;
									}
									state.dateput.openLevel("empty");
									state.dateput.append(value_end, this);
									if (first_date) {
										state.dateput.current.value().blobs[0].strings.prefix = "";
									}
									if (bc) {
										state.dateput.append(bc);
									}
									if (ad) {
										state.dateput.append(ad);
									}
									state.dateput.closeLevel();
								}
							}
						}
					} else {
						state.output.append(value, this);
					}

					if (bc) {
						state.output.append(bc);
					}
					if (ad) {
						state.output.append(ad);
					}
					state.output.closeLevel();
					//state.output.endTag();
				} else if ("month" === this.strings.name) {
					//
					// No value for this target variable
					//
					if (state.tmp.date_object["season"]) {
						value = ""+state.tmp.date_object["season"];
						if (value && value.match(/^[1-4]$/)){
							state.output.append(state.getTerm(("season-0"+value)), this);
						} else if (value) {
							state.output.append(value, this);
						}
					}
				}
				state.tmp.value = new Array();
				if (!state.opt.has_year_suffix && "year" === this.strings.name) {
					if (state.registry.registry[Item.id] && state.registry.registry[Item.id].disambig[2]) {
						var num = parseInt(state.registry.registry[Item.id].disambig[2], 10);
						var number = new CSL.NumericBlob(num, this);
						var formatter = new CSL.Util.Suffixator(CSL.SUFFIX_CHARS);
						number.setFormatter(formatter);
						state.output.append(number, "literal");
					}
				}

			};
			this["execs"].push(render_date_part);
			if ("undefined" === typeof this.strings["range-delimiter"]) {
				this.strings["range-delimiter"] = "-";
			}
			target.push(this);
		}
	}
};


