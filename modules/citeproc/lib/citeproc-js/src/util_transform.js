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

/*
 * Fields can be transformed by translation/transliteration, or by
 * abbreviation.  Transformations are performed in that order.
 *
 * Renderings of original, translated or transliterated content
 * (followed by abbreviation if requested) are placed in the primary
 * output slot or the (implicitly punctuated) secondary and tertiary
 * output slots according to the settings registered in the
 * state.opt['cite-lang-prefs'] arrays. The array has five segments:
 * 'persons', 'institutions', 'titles', 'publishers', and
 * 'places'. Each segment always contains at least one item, and may
 * hold values 'orig', 'translit' or 'translat'. The array defaults to
 * a single item 'orig'.
 *
 * All multilingual variables are associated with segments,
 * with the exception of 'edition' and 'genre'. These two
 * exceptions are always rendered with the first matching
 * language form found in state.opt['locale-translit'] or, if
 * composing a sort key, state.opt['locale-sort']. No secondary
 * slot rendering is performed for this two variables.
 *
 * The balance of multilingual variables are rendered with
 * the first matching value in the transform locales spec
 * (no transform, state.opt['locale-translit'], or 
 * state.opt['locale-translat']) mapped to the target
 * slot.
 *
 * Full primary+secondary+tertiary rendering is performed only in
 * note-style citations and the bibliography.  In-text citations are
 * rendered in the primary output slot only, following the same spec
 * parameters.
 *
 *   Optional setters:
 *     .setAbbreviationFallback(); fallback flag
 *       (if true, a failed abbreviation will fallback to long)
 *
 *     .setAlternativeVariableName(): alternative variable name in Item,
 *       for use as a fallback abbreviation source
 *
 * Translation/transliteration
 *
 *   Optional setter:
 *     .setTransformFallback():
 *       default flag (if true, the original field value will be used as a fallback)
 *
 * The getTextSubField() method may be used to obtain a string transform
 * of a field, without abbreviation, as needed for setting sort keys
 * (for example).
 *
 */

CSL.Transform = function (state) {
    var debug = false, abbreviations, token, fieldname, abbrev_family, opt;

    // Abbreviation families
    this.abbrevs = {};
    this.abbrevs["default"] = new CSL.AbbreviationSegments();

    // Initialization method
    function init(mytoken, myfieldname, myabbrev_family) {
        token = mytoken;
        fieldname = myfieldname;
        abbrev_family = myabbrev_family;
        opt = {
            abbreviation_fallback: false,
            alternative_varname: false,
            transform_fallback: false
        };
    }
    this.init = init;

    // Internal function
    function abbreviate(state, Item, altvar, basevalue, myabbrev_family, use_field) {
        var value;

        if (!myabbrev_family) {
            return basevalue;
        }

        if (["publisher-place", "event-place"].indexOf(myabbrev_family) > -1) {
            myabbrev_family = "place";
        }

        if (["publisher", "authority"].indexOf(myabbrev_family) > -1) {
            myabbrev_family = "institution-part";
        }

        if (["genre"].indexOf(myabbrev_family) > -1) {
            myabbrev_family = "title";
        }

        if (["title-short"].indexOf(myabbrev_family) > -1) {
            myabbrev_family = "title";
        }

        // Lazy retrieval of abbreviations.
        value = "";
        if (state.sys.getAbbreviation) {
            var jurisdiction = state.transform.loadAbbreviation(Item.jurisdiction, myabbrev_family, basevalue);

            // XXX Need a fallback mechanism here. Other to default.
            if (state.transform.abbrevs[jurisdiction][myabbrev_family] && basevalue && state.sys.getAbbreviation) {
                if (state.transform.abbrevs[jurisdiction][myabbrev_family][basevalue]) {
                    value = state.transform.abbrevs[jurisdiction][myabbrev_family][basevalue];
                }
            }
        }
        if (!value && altvar && Item[altvar] && use_field) {
            value = Item[altvar];
        }
        if (!value) {
            value = basevalue;
        }
        return value;
    }

    // Internal function
    function getTextSubField(Item, field, locale_type, use_default, stopOrig) {
        var m, lst, opt, o, oo, pos, key, ret, len, myret, opts;
        var usedOrig = stopOrig;

        if (!Item[field]) {
            return {name:"", usedOrig:stopOrig};
        }
        ret = {name:"", usedOrig:stopOrig};

        opts = state.opt[locale_type];
        if (locale_type === 'locale-orig') {
            if (stopOrig) {
                ret = {name:"", usedOrig:stopOrig};
            } else {
                ret = {name:Item[field], usedOrig:false};
            }
            return ret;
        } else if (use_default && ("undefined" === typeof opts || opts.length === 0)) {
			// If we want the original, or if we don't have any specific guidance and we 
            // definitely want output, just return the original value.
            return {name:Item[field], usedOrig:true};
        }

        for (var i = 0, ilen = opts.length; i < ilen; i += 1) {
            // Fallback from more to less specific language tag
            opt = opts[i];
            o = opt.split(/[\-_]/)[0];
            if (opt && Item.multi && Item.multi._keys[field] && Item.multi._keys[field][opt]) {
                ret.name = Item.multi._keys[field][opt];
                break;
            } else if (o && Item.multi && Item.multi._keys[field] && Item.multi._keys[field][o]) {
                ret.name = Item.multi._keys[field][o];
                break;
            }
        }
        if (!ret.name && use_default) {
            ret = {name:Item[field], usedOrig:true};
        }
        return ret;
    }

    //
    function setAbbreviationFallback(b) {
        opt.abbreviation_fallback = b;
    }
    this.setAbbreviationFallback = setAbbreviationFallback;

    //
    function setAlternativeVariableName(s) {
        opt.alternative_varname = s;
    }
    this.setAlternativeVariableName = setAlternativeVariableName;

    //
    function setTransformFallback(b) {
        opt.transform_fallback = b;
    }
    this.setTransformFallback = setTransformFallback;

    // Setter for abbreviation lists
    // This initializes a single abbreviation based on known
    // data.
    function loadAbbreviation(jurisdiction, category, orig) {
        var pos, len;
        if (!jurisdiction) {
            jurisdiction = "default";
        }
        if (!orig) {
            return jurisdiction;
        }
        // The getAbbreviation() function should check the
        // external DB for the content key. If a value exists
        // in this[category] and no value exists in DB, the entry
        // in memory is left untouched. If a value does exist in
        // DB, the memory value is created.
        //
        // See testrunner_stdrhino.js for an example.
        if (state.sys.getAbbreviation) {
            if (!this.abbrevs[jurisdiction]) {
                this.abbrevs[jurisdiction] = new CSL.AbbreviationSegments();
            }
            if (!this.abbrevs[jurisdiction][category][orig]) {
                // jurisdiction could change to "default"
                state.sys.getAbbreviation(state.opt.styleID, this.abbrevs, jurisdiction, category, orig);
            }
        }
        return jurisdiction;
    }
    this.loadAbbreviation = loadAbbreviation;

    function publisherCheck (tok, Item, primary, myabbrev_family) {
        var varname = tok.variables[0];
        if (state.publisherOutput && primary) {
            if (["publisher","publisher-place"].indexOf(varname) === -1) {
                return false;
            } else {
                // In this case, the publisher bundle will be rendered
                // at the close of the group, by the closing group node.
                state.publisherOutput[varname + "-token"] = tok;
                state.publisherOutput.varlist.push(varname);
                var lst = primary.split(/;\s*/);
                if (lst.length === state.publisherOutput[varname + "-list"].length) {
                    state.publisherOutput[varname + "-list"] = lst;
                }
                // XXX Abbreviate each of the items in the list here!
                for (var i = 0, ilen = lst.length; i < ilen; i += 1) {
					// myabbrev_family just turns abbreviation on if it has a value (any value)
                    lst[i] = abbreviate(state, Item, false, lst[i], myabbrev_family, true);
                }
                state.tmp[varname + "-token"] = tok;
                return true;
            }
        }
        return false;
    }

    // Return function appropriate to selected options
    function getOutputFunction(variables) {
		// var mytoken;
        var myabbrev_family, myfieldname, abbreviation_fallback, alternative_varname, transform_locale, transform_fallback, getTextSubfield;

        // Freeze mandatory values
        //mytoken = CSL.Util.cloneToken(token); // the token isn't needed, is it?

		// the Abbreviation Family set by init(), if any.
        myabbrev_family = abbrev_family;

		// The fieldname set by init().
        myfieldname = fieldname;

        // Freeze option values
		// These are also from init(), adjusted by configuration helper functions.
        abbreviation_fallback = opt.abbreviation_fallback;
        alternative_varname = opt.alternative_varname;
        transform_fallback = opt.transform_fallback;

        // Set the primary_locale and secondary_locale lists appropriately.
		// No instance helper function for this; everything can be derived
		// from processor settings and rendering context.

		var localesets;
        var langPrefs = CSL.LangPrefsMap[myfieldname];
        if (!langPrefs) {
            localesets = false;
        } else {
            localesets = state.opt['cite-lang-prefs'][langPrefs];
        }

        return function (state, Item, item, usedOrig) {
            var primary, secondary, tertiary, primary_tok, group_tok, key;
            if (!variables[0]) {
                return null;
            }
            
		    var slot = {primary:false, secondary:false, tertiary:false};
		    if (state.tmp.area.slice(-5) === "_sort") {
			    slot.primary = 'locale-sort';
		    } else {
			    if (localesets) {
				    var slotnames = ["primary", "secondary", "tertiary"];
				    for (var i = 0, ilen = slotnames.length; i < ilen; i += 1) {
					    if (localesets.length - 1 <  i) {
						    break;
					    }
                        if (localesets[i]) {
						    slot[slotnames[i]] = 'locale-' + localesets[i];
                        }
				    }
			    } else {
					slot.primary = 'locale-translat';
			    }
            }
            
			if ((state.tmp.area !== "bibliography"
				 && !(state.tmp.area === "citation"
					  && state.opt.xclass === "note"
					  && item && !item.position))
				|| myabbrev_family) {
                
				slot.secondary = false;
				slot.tertiary = false;
			}
            
            // Problem for multilingual: we really should be
            // checking for sanity on the basis of the output
            // strings to be actually used. (also below)
            if (state.tmp["publisher-list"]) {
                if (variables[0] === "publisher") {
                    state.tmp["publisher-token"] = this;
                } else if (variables[0] === "publisher-place") {
                    state.tmp["publisher-place-token"] = this;
                }
                return null;
            }

			// True is for transform fallback
            var res = getTextSubField(Item, myfieldname, slot.primary, true);
            primary = res.name;

            if (publisherCheck(this, Item, primary, myabbrev_family)) {
                return null;
            }

			// No fallback for secondary and tertiary
			secondary = false;
			tertiary = false;
			if (slot.secondary) {
				res = getTextSubField(Item, myfieldname, slot.secondary, false, res.usedOrig);
                secondary = res.name;
			}
			if (slot.tertiary) {
				res = getTextSubField(Item, myfieldname, slot.tertiary, false, res.usedOrig);
                tertiary = res.name;
			}
            
            // Abbreviate if requested and if poss.
			// (We don't yet control for the possibility that full translations may not
			// be provided on the alternative variable.)
            if (myabbrev_family) {
                primary = abbreviate(state, Item, alternative_varname, primary, myabbrev_family, true);
                secondary = abbreviate(state, Item, false, secondary, myabbrev_family, true);
                tertiary = abbreviate(state, Item, false, tertiary, myabbrev_family, true);
            }
            
            if ("demote" === this["leading-noise-words"]) {
                primary = CSL.demoteNoiseWords(state, primary);
                secondary = CSL.demoteNoiseWords(state, secondary);
                tertiary = CSL.demoteNoiseWords(state, tertiary);
            }
            if (secondary || tertiary) {
                primary_tok = CSL.Util.cloneToken(this);
                primary_tok.strings.suffix = "";
                state.output.append(primary, primary_tok);
                
                group_tok = new CSL.Token();
                group_tok.strings.prefix = " [";
                group_tok.strings.delimiter = ", ";
                group_tok.strings.suffix = "]" + this.strings.suffix;
				state.output.openLevel(group_tok);
				if (secondary) {
					state.output.append(secondary);
				}
				if (tertiary) {
					state.output.append(tertiary);
				}
				state.output.closeLevel();
            } else {
                state.output.append(primary, this);
            }
            return null;
        };
        /*
        } else {
            return function (state, Item) {
                var primary;
                if (!variables[0]) {
                    return null;
                }
                primary = getTextSubField(Item, myfieldname, transform_locale, transform_fallback);

                // Factor this out
                if (publisherCheck(this, Item, primary)) {
                    return null;
                } else {
                    if ("demote" === this["leading-noise-words"]) {
                        primary = CSL.demoteNoiseWords(state, primary);
                    }
                    // Safe, because when state.tmp["publisher-list"] exists,
                    // the variable must be one of publisher or publisher-place.
                    primary = abbreviate(state, Item, alternative_varname, primary, myabbrev_family, true);
                    state.output.append(primary, this);
                }
                return null;
            };
        }
        */
    }
    this.getOutputFunction = getOutputFunction;

    // The name transform code is placed here to keep similar things
    // in one place.  Obviously this module could do with a little
    // tidying up.


    function getHereinafter (Item) {
        var hereinafter_author_title = [];
        if (state.tmp.first_name_string) {
            hereinafter_author_title.push(state.tmp.first_name_string);
        }
        if (Item.title) {
            hereinafter_author_title.push(Item.title);
        }
        var hereinafter_metadata = [];
        if (Item.type) {
            hereinafter_metadata.push("type:" + Item.type);
        }
        if (Item.issued) {
            var date = [];
            for (var j = 0, jlen = CSL.DATE_PARTS.length; j < jlen; j += 1) {
                if (Item.issued[CSL.DATE_PARTS[j]]) {
                    var element =  Item.issued[CSL.DATE_PARTS[j]];
                    while (element.length < 2) {
                        element = "0" + element;
                    }
                    date.push(element);
                }
            }
            date = date.join("-");
            if (date) {
                hereinafter_metadata.push("date:" + date);
            }
        }
        if (Item.jurisdiction) {
            hereinafter_metadata.push("jurisdiction:" + Item.jurisdiction);
        }
        hereinafter_metadata = hereinafter_metadata.join(", ");
        if (hereinafter_metadata) {
            hereinafter_metadata = " [" + hereinafter_metadata + "]";
        }
        var hereinafter_key = hereinafter_author_title.join(", ") + hereinafter_metadata;
        return hereinafter_key;
    }
    this.getHereinafter = getHereinafter;
};



