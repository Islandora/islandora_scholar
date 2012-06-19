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

CSL.NameOutput.prototype.renderAllNames = function () {

    // Note that et-al/ellipsis parameters are set on the basis
    // of rendering order through the whole cite.
    var pos = this.nameset_base;
    for (var i = 0, ilen = this.variables.length; i < ilen; i += 1) {
        var v = this.variables[i];
        if (this.freeters[v].length) {
            this.freeters[v] = this._renderPersonalNames(this.freeters[v], pos);
            pos += 1;
        }
        if (this.institutions[v].length) {
            pos += 1;
        }
        for (var j = 0, jlen = this.institutions[v].length; j < jlen; j += 1) {
            this.persons[v][j] = this._renderPersonalNames(this.persons[v][j], pos);
            pos += 1;
        }
    }
    this.renderInstitutionNames();
};

CSL.NameOutput.prototype.renderInstitutionNames = function () {
    // Institutions are split to string list as
    // this.institutions[v]["long"] and this.institutions[v]["short"]
    for (var i = 0, ilen = this.variables.length; i < ilen; i += 1) {
        var v = this.variables[i];
        for (var j = 0, jlen = this.institutions[v].length; j < jlen; j += 1) {
            var institution, institution_short, institution_long, short_style, long_style;

            var name = this.institutions[v][j];

            // XXX Start here for institutions
            // Figure out the three segments: primary, secondary, tertiary
            var j, ret, optLangTag, jlen, key, localesets;
            if (this.state.tmp.extension) {
                localesets = ["sort"];
            } else if (name.isInstitution) {
                // Will never hit this in this function, but preserving
                // in case we factor this out.
                localesets = this.state.opt['cite-lang-prefs'].institutions;
            } else {
                localesets = this.state.opt['cite-lang-prefs'].persons;
            }

            slot = {primary:false,secondary:false,tertiary:false};
	        if (localesets) {
		        var slotnames = ["primary", "secondary", "tertiary"];
		        for (var k = 0, klen = slotnames.length; k < klen; k += 1) {
			        if (localesets.length - 1 <  j) {
				        break;
			        }
                    if (localesets[k]) {
			            slot[slotnames[k]] = 'locale-' + localesets[k];
                    }
		        }
	        } else {
		        slot.primary = 'locale-translat';
	        }
	        if (this.state.tmp.area !== "bibliography"
		        && !(this.state.tmp.area === "citation"
			         && this.state.opt.xclass === "note"
			         && this.item && !this.item.position)) {
                
		        slot.secondary = false;
		        slot.tertiary = false;
	        }
            // Get normalized name object for a start.
            // true invokes fallback
            var res;
            res = this.getName(name, slot.primary, true);
            var primary = res.name;
            var usedOrig = res.usedOrig;
            if (primary) {
                primary = this.fixupInstitution(primary, v, j);
            }

			secondary = false;
			if (slot.secondary) {
                res = this.getName(name, slot.secondary, false, usedOrig);
                secondary = res.name;
                usedOrig = res.usedOrig;
                if (secondary) {
				    secondary = this.fixupInstitution(secondary, v, j);
                }
			}
            //Zotero.debug("XXX [2] secondary: "+secondary["long"].literal+", slot.secondary: "+slot.secondary);
			tertiary = false;
			if (slot.tertiary) {
                res = this.getName(name, slot.tertiary, false, usedOrig);
                tertiary = res.name;
                if (tertiary) {
				    tertiary = this.fixupInstitution(tertiary, v, j);
                }
			}
            switch (this.institution.strings["institution-parts"]) {
            case "short":
                // No multilingual for pure short form institution names.
                if (primary["short"].length) {
                    short_style = this._getShortStyle();
                    institution = [this._renderOneInstitutionPart(primary["short"], short_style)];
                } else {
                    // Fail over to long.

                    long_style = this._getLongStyle(primary, v, j);
                    institution = [this._renderOneInstitutionPart(primary["long"], long_style)];
                }
                break;
            case "short-long":
                long_style = this._getLongStyle(primary, v, j);
                short_style = this._getShortStyle();
                institution_short = this._renderOneInstitutionPart(primary["short"], short_style);
                // true is to include multilingual supplement
                institution_long = this._composeOneInstitutionPart([primary, secondary, tertiary], long_style);
                institution = [institution_short, institution_long];
                break;
            case "long-short":
                long_style = this._getLongStyle(primary, v, j);
                short_style = this._getShortStyle();
                institution_short = this._renderOneInstitutionPart(primary["short"], short_style);
                // true is to include multilingual supplement
                institution_long = this._composeOneInstitutionPart([primary, secondary, tertiary], long_style, true);
                institution = [institution_long, institution_short];
                break;
            default:
                long_style = this._getLongStyle(primary, v, j);
                // true is to include multilingual supplement
                institution = [this._composeOneInstitutionPart([primary, secondary, tertiary], long_style)];
                break;
            }
            this.institutions[v][j] = this._join(institution, "");
        }
    }
};

CSL.NameOutput.prototype._composeOneInstitutionPart = function (names, style) {
    var primary = false, secondary = false, tertiary = false;
    if (names[0]) {
        primary = this._renderOneInstitutionPart(names[0]["long"], style);
    }
    if (names[1]) {
        secondary = this._renderOneInstitutionPart(names[1]["long"], style);
    }
    if (names[2]) {
        tertiary = this._renderOneInstitutionPart(names[2]["long"], style);
    }
    // Compose
    var institutionblob;
    if (secondary || tertiary) {
        var multiblob = this._join([secondary, tertiary], ", ");
        var group_tok = new CSL.Token();
        group_tok.strings.prefix = " [";
        group_tok.strings.suffix = "]";
        this.state.output.openLevel(group_tok);
        this.state.output.append(multiblob);
        this.state.output.closeLevel();
        multiblob = this.state.output.pop();
        institutionblob = this._join([primary, multiblob], "");
    } else {
        institutionblob = primary;
    }
    return institutionblob;
}

CSL.NameOutput.prototype._renderOneInstitutionPart = function (blobs, style) {
    for (var i = 0, ilen = blobs.length; i < ilen; i += 1) {
        if (blobs[i]) {
            var str = blobs[i];
            // XXXXX Cut-and-paste code in multiple locations. This code block should be
            // collected in a function.
            // Tag: strip-periods-block
            if (this.state.tmp.strip_periods) {
                str = str.replace(/\./g, "");
            } else {
                for (var j = 0, jlen = style.decorations.length; j < jlen; j += 1) {
                    if ("@strip-periods" === style.decorations[j][0] && "true" === style.decorations[j][1]) {
                        str = str.replace(/\./g, "");
                        break;
                    }
                }
            }
            //this.state.output.append(blobs[i], style, true);
            this.state.tmp.group_context.value()[2] = true;
            this.state.tmp.can_substitute.replace(false, CSL.LITERAL);
            this.state.output.append(str, style, true);
            blobs[i] = this.state.output.pop();
        }
    }
    if ("undefined" === typeof this.institution.strings["part-separator"]) {
        this.institution.strings["part-separator"] = this.name.strings.delimiter;
    }
    return this._join(blobs, this.institution.strings["part-separator"]);
};

CSL.NameOutput.prototype._renderPersonalNames = function (values, pos) {
    //
    var ret = false;
    if (values.length) {
        var names = [];
        for (var i = 0, ilen = values.length; i < ilen; i += 1) {
            var name = values[i];
            
            // XXX We'll start here with attempts.
            // Figure out the three segments: primary, secondary, tertiary
            var j, ret, optLangTag, jlen, key, localesets;
            if (this.state.tmp.extension) {
                localesets = ["sort"];
            } else if (name.isInstitution) {
                // Will never hit this in this function, but preserving
                // in case we factor this out.
                localesets = this.state.opt['cite-lang-prefs'].institutions;
            } else {
                localesets = this.state.opt['cite-lang-prefs'].persons;
            }
            slot = {primary:false,secondary:false,tertiary:false};
	        if (localesets) {
		        var slotnames = ["primary", "secondary", "tertiary"];
		        for (var j = 0, jlen = slotnames.length; j < jlen; j += 1) {
			        if (localesets.length - 1 <  j) {
				        break;
			        }
			        slot[slotnames[j]] = 'locale-' + localesets[j];
		        }
	        } else {
		        slot.primary = 'locale-translat';
	        }
	        if (this.state.tmp.sort_key_flag || (this.state.tmp.area !== "bibliography"
		        && !(this.state.tmp.area === "citation"
			         && this.state.opt.xclass === "note"
			         && this.item && !this.item.position))) {
                
		        slot.secondary = false;
		        slot.tertiary = false;
	        }

            // primary
            // true is for fallback
            var res = this.getName(name, slot.primary, true);
            var primary = this._renderOnePersonalName(res.name, pos, i);
			secondary = false;
			if (slot.secondary) {
                res = this.getName(name, slot.secondary, false, res.usedOrig);
                if (res.name) {
				    secondary = this._renderOnePersonalName(res.name, pos, i);
                }
			}
			tertiary = false;
			if (slot.tertiary) {
                res = this.getName(name, slot.tertiary, false, res.usedOrig);
                if (res.name) {
				    tertiary = this._renderOnePersonalName(res.name, pos, i);
                }
			}
            // Now compose them to a unit
            var personblob;
            if (secondary || tertiary) {
                var multiblob = this._join([secondary, tertiary], ", ");
                var group_tok = new CSL.Token();
                group_tok.strings.prefix = " [";
                group_tok.strings.suffix = "]";
                this.state.output.openLevel(group_tok);
                this.state.output.append(multiblob);
                this.state.output.closeLevel();
                multiblob = this.state.output.pop();
                personblob = this._join([primary, multiblob], "");
            } else {
                personblob = primary;
            }
            names.push(personblob);
        }
        ret = this.joinPersons(names, pos);
    }
    return ret;
};

CSL.NameOutput.prototype._renderOnePersonalName = function (value, pos, i) {
    var name = value;
    var dropping_particle = this._droppingParticle(name, pos);
    var family = this._familyName(name);
    var non_dropping_particle = this._nonDroppingParticle(name);
    var given = this._givenName(name, pos, i);
    var suffix = this._nameSuffix(name);
    if (this._isShort(pos, i)) {
        dropping_particle = false;
        given = false;
        suffix = false;
    }
    var sort_sep = this.name.strings["sort-separator"];
    if (!sort_sep) {
        sort_sep = "";
    }
    var suffix_sep;
    if (name["comma-suffix"]) {
        suffix_sep = ", ";
    } else {
        suffix_sep = " ";
    }
    var romanesque = name.family.match(CSL.ROMANESQUE_REGEXP);
    var blob, merged, first, second;
    if (!romanesque) {
        // XXX handle affixes for given and family
        blob = this._join([non_dropping_particle, family, given], "");
    } else if (name["static-ordering"]) { // entry likes sort order
        blob = this._join([non_dropping_particle, family, given], " ");
    } else if (this.state.tmp.sort_key_flag) {
        // ok with no affixes here
        if (this.state.opt["demote-non-dropping-particle"] === "never") {
            first = this._join([non_dropping_particle, family, dropping_particle], " ");
            merged = this._join([first, given], " ");
            blob = this._join([merged, suffix], " ");
        } else {
            second = this._join([given, dropping_particle, non_dropping_particle], " ");
            merged = this._join([family, second], " ");
            blob = this._join([merged, suffix], " ");
        }
    } else if (this.name.strings["name-as-sort-order"] === "all" || (this.name.strings["name-as-sort-order"] === "first" && i === 0)) {
        //
        // Discretionary sort ordering and inversions
        //
        if (["Lord", "Lady"].indexOf(name.given) > -1) {
            sort_sep = ", ";
        }
        if (["always", "display-and-sort"].indexOf(this.state.opt["demote-non-dropping-particle"]) > -1) {
            // Drop non-dropping particle
            //second = this._join([given, dropping_particle, non_dropping_particle], " ");
            second = this._join([given, dropping_particle], (name["comma-dropping-particle"] + " "));
            second = this._join([second, non_dropping_particle], " ");
            if (second && this.given) {
                second.strings.prefix = this.given.strings.prefix;
                second.strings.suffix = this.given.strings.suffix;
            }
            if (family && this.family) {
                family.strings.prefix = this.family.strings.prefix;
                family.strings.suffix = this.family.strings.suffix;
            }
            merged = this._join([family, second], sort_sep);
            blob = this._join([merged, suffix], sort_sep);
        } else {
            // Don't drop particle.
            first = this._join([non_dropping_particle, family], " ");
            if (first && this.family) {
                first.strings.prefix = this.family.strings.prefix;
                first.strings.suffix = this.family.strings.suffix;
            }

            second = this._join([given, dropping_particle], (name["comma-dropping-particle"] + " "));
            //second = this._join([given, dropping_particle], " ");
            if (second && this.given) {
                second.strings.prefix = this.given.strings.prefix;
                second.strings.suffix = this.given.strings.suffix;
            }

            merged = this._join([first, second], sort_sep);
            blob = this._join([merged, suffix], sort_sep);
        }
    } else { // plain vanilla
        if (name["dropping-particle"] && name.family && !name["non-dropping-particle"]) {
            if (["'","\u02bc","\u2019"].indexOf(name["dropping-particle"].slice(-1)) > -1) {
                family = this._join([dropping_particle, family], "");
                dropping_particle = false;
            }
        }
        second = this._join([dropping_particle, non_dropping_particle, family], " ");
        second = this._join([second, suffix], suffix_sep);
        if (second && this.family) {
            second.strings.prefix = this.family.strings.prefix;
            second.strings.suffix = this.family.strings.suffix;
        }
        if (given && this.given) {
            given.strings.prefix = this.given.strings.prefix;
            given.strings.suffix = this.given.strings.suffix;
        }
        if (second.strings.prefix) {
            name["comma-dropping-particle"] = "";
        }
        blob = this._join([given, second], (name["comma-dropping-particle"] + " "));
    }
    // XXX Just generally assume for the present that personal names render something
    this.state.tmp.group_context.value()[2] = true;
    this.state.tmp.can_substitute.replace(false, CSL.LITERAL);
    // notSerious
    //this.state.output.append(blob, "literal", true);
    //var ret = this.state.output.pop();
    this.state.tmp.name_node.children.push(blob);
    return blob;
};

CSL.NameOutput.prototype._isShort = function (pos, i) {
    if (0 === this.state.tmp.disambig_settings.givens[pos][i]) {
        return true;
    } else {
        return false;
    }
};

/*
        // Do not include given name, dropping particle or suffix in strict short form of name

        // initialize if appropriate
*/

// Input names should be touched by _normalizeNameInput()
// exactly once: this is not idempotent.
CSL.NameOutput.prototype._normalizeNameInput = function (value) {
    var name = {
        literal:value.literal,
        family:value.family,
        isInstitution:value.isInstitution,
        given:value.given,
        suffix:value.suffix,
        "comma-suffix":value["comma-suffix"],
        "non-dropping-particle":value["non-dropping-particle"],
        "dropping-particle":value["dropping-particle"],
        "static-ordering":value["static-ordering"],
        "parse-names":value["parse-names"],
        "comma-dropping-particle": "",
        block_initialize:value.block_initialize,
        multi:value.multi
    };
    this._parseName(name);
    return name;
};

// _transformNameset() replaced with enhanced transform.name().

CSL.NameOutput.prototype._stripPeriods = function (tokname, str) {
    var decor_tok = this[tokname + "_decor"];
    if (str) {
        if (this.state.tmp.strip_periods) {
            str = str.replace(/\./g, "");
        } else  if (decor_tok) {
            for (var i = 0, ilen = decor_tok.decorations.length; i < ilen; i += 1) {
                if ("@strip-periods" === decor_tok.decorations[i][0] && "true" === decor_tok.decorations[i][1]) {
                    str = str.replace(/\./g, "");
                    break;
                }
            }
        }
    }
    return str;
};

CSL.NameOutput.prototype._nonDroppingParticle = function (name) {
    var str = this._stripPeriods("family", name["non-dropping-particle"]);
    if (this.state.output.append(str, this.family_decor, true)) {
        return this.state.output.pop();
    }
    return false;
};

CSL.NameOutput.prototype._droppingParticle = function (name, pos) {
    var str = this._stripPeriods("given", name["dropping-particle"]);
    if (name["dropping-particle"] && name["dropping-particle"].match(/^et.?al[^a-z]$/)) {
        if (this.name.strings["et-al-use-last"]) {
            this.etal_spec[pos] = 2;
        } else {
            this.etal_spec[pos] = 1;
        }
        name["comma-dropping-particle"] = "";
    } else if (this.state.output.append(str, this.given_decor, true)) {
        return this.state.output.pop();
    }
    return false;
};

CSL.NameOutput.prototype._familyName = function (name) {
    var str = this._stripPeriods("family", name.family);
    if (this.state.output.append(str, this.family_decor, true)) {
        return this.state.output.pop();
    }
    return false;
};

CSL.NameOutput.prototype._givenName = function (name, pos, i) {

    if (this.name.strings.initialize === false) {
        if (name.family && name.given && this.name.strings.initialize === false) {
            name.given = CSL.Util.Names.initializeWith(this.state, name.given, this.name.strings["initialize-with"], true);
        }
        name.given = CSL.Util.Names.unInitialize(this.state, name.given);
    } else {
        if (name.family && 1 === this.state.tmp.disambig_settings.givens[pos][i] && !name.block_initialize) {
            var initialize_with = this.name.strings["initialize-with"];
            name.given = CSL.Util.Names.initializeWith(this.state, name.given, initialize_with);
        } else {
            name.given = CSL.Util.Names.unInitialize(this.state, name.given);
        }
    }

    var str = this._stripPeriods("given", name.given);
    if (this.state.output.append(str, this.given_decor, true)) {
        return this.state.output.pop();
    }
    return false;
};

CSL.NameOutput.prototype._nameSuffix = function (name) {

    var str = name.suffix;

    if ("string" === typeof this.name.strings["initialize-with"]) {
        str = CSL.Util.Names.initializeWith(this.state, name.suffix, this.name.strings["initialize-with"], true);
    }

    str = this._stripPeriods("family", str);

    if (this.state.output.append(str, "empty", true)) {
        return this.state.output.pop();
    }
    return false;
};

CSL.NameOutput.prototype._getLongStyle = function (name, v, i) {
    var long_style, short_style;
    if (name["short"].length) {
        if (this.institutionpart["long-with-short"]) {
            long_style = this.institutionpart["long-with-short"];
        } else {
            long_style = this.institutionpart["long"];
        }
    } else {
        long_style = this.institutionpart["long"];
    }
    if (!long_style) {
        long_style = new CSL.Token();
    }
    return long_style;
};

CSL.NameOutput.prototype._getShortStyle = function () {
    var short_style;
    if (this.institutionpart["short"]) {
        short_style = this.institutionpart["short"];
    } else {
        short_style = new CSL.Token();
    }
    return short_style;
};

CSL.NameOutput.prototype._parseName = function (name) {
    var m, idx;
    if (!name["parse-names"] && "undefined" !== typeof name["parse-names"]) {
        return name;
    }
    if (name.family && !name.given && name.isInstitution) {
        name.literal = name.family;
        name.family = undefined;
        name.isInstitution = undefined;
    }
    var noparse;
    if (name.family 
        && (name.family.slice(0, 1) === '"' && name.family.slice(-1) === '"')
        || (!name["parse-names"] && "undefined" !== typeof name["parse-names"])) {

        name.family = name.family.slice(1, -1);
        noparse = true;
        name["parse-names"] = 0;
    } else {
        noparse = false;
    }
    if (!name["non-dropping-particle"] && name.family && !noparse) {
        m = name.family.match(/^((?:[a-z][ \'\u2019a-z]*[\s+|\'\u2019]|[DVL][^ ]\s+[a-z]*\s*|[DVL][^ ][^ ]\s+[a-z]*\s*))/);
        if (m) {
            name.family = name.family.slice(m[1].length);
            name["non-dropping-particle"] = m[1].replace(/\s+$/, "");

        }
    }
    if (!name.suffix && name.given) {
        m = name.given.match(/(\s*,!*\s*)/);
        if (m) {
            idx = name.given.indexOf(m[1]);
            var possible_suffix = name.given.slice(idx + m[1].length);
            var possible_comma = name.given.slice(idx, idx + m[1].length).replace(/\s*/g, "");
            if (possible_suffix.length <= 3) {
                if (possible_comma.length === 2) {
                    name["comma-suffix"] = true;
                }
                name.suffix = possible_suffix;
            } else if (!name["dropping-particle"] && name.given) {
                name["dropping-particle"] = possible_suffix;
                name["comma-dropping-particle"] = ",";
            }
            name.given = name.given.slice(0, idx);
        }
    }
    if (!name["dropping-particle"] && name.given) {
        m = name.given.match(/(\s+)([a-z][ \'\u2019a-z]*)$/);
        if (m) {
            name.given = name.given.slice(0, (m[1].length + m[2].length) * -1);
            name["dropping-particle"] = m[2];
        }
    }
};


/*
 * Return a single name object
  */

// The interface is a mess, but this should serve.

CSL.NameOutput.prototype.getName = function (name, slotLocaleset, fallback, stopOrig) {

    // Needs to tell us whether we used orig or not.
    
    if (stopOrig && slotLocaleset === 'locale-orig') {
        return {name:false,usedOrig:stopOrig};
    }

    // Normalize to string
    if (!name.family) {
        name.family = "";
    }
    if (!name.given) {
        name.given = "";
    }
    //
    // Optionally add a static-ordering toggle for non-roman, non-Cyrillic
    // names, based on the headline values.
    //
    var static_ordering_freshcheck = false;
    var block_initialize = false;
    var transliterated = false;
    var static_ordering_val = this.getStaticOrder(name);

    // The stuff below should move to this._getName()?

    // The code below is run up to three times, to get the forms
    // for each name-form segment.
    
    //
    // Step through the requested languages in sequence
    // until a match is found
    //
    var foundTag = true;
    if (slotLocaleset !== 'locale-orig') {
        foundTag = false;
        if (name.multi) {
            var langTags = this.state.opt[slotLocaleset]
            for (i = 0, ilen = langTags.length; i < ilen; i += 1) {
                langTag = langTags[i];
                if (name.multi._key[langTag]) {
                    foundTag = true;
                    name = name.multi._key[langTag];
                    transliterated = true;
                    if (!this.state.opt['locale-use-original-name-format'] && false) {
                        // We may reintroduce this option later, but for now, pretend
                        // it's always turned on.
                        static_ordering_freshcheck = true;
                    } else {
                        // Quash initialize-with if original was non-romanesque
                        // and we are trying to preserve the original formatting
                        // conventions.
                        // (i.e. supply as much information as possible if
                        // the transliteration spans radically different
                        // writing conventions)
                        if ((name.family.replace('"','','g') + name.given).match(CSL.ROMANESQUE_REGEXP)) {
                            block_initialize = true;
                        }
                    }
                    break;
                }
            }
        }
    }
    
    if (!fallback && !foundTag) {
        return {name:false,usedOrig:stopOrig};
    }
    
    // Normalize to string (again)
    if (!name.family) {
        name.family = "";
    }
    if (!name.given) {
        name.given = "";
    }
    
    
    // var clone the item before writing into it
    name = {
        family:name.family,
        given:name.given,
        "non-dropping-particle":name["non-dropping-particle"],
        "dropping-particle":name["dropping-particle"],
        suffix:name.suffix,
        "static-ordering":static_ordering_val,
        "parse-names":name["parse-names"],
        "comma-suffix":name["comma-suffix"],
        "comma-dropping-particle":name["comma-dropping-particle"],
        transliterated:transliterated,
        block_initialize:block_initialize,
        literal:name.literal,
        isInstitution:name.isInstitution,
    };
    if (static_ordering_freshcheck &&
        !this.getStaticOrder(name, true)) {
        
        name["static-ordering"] = false;
    }
    
    if (!name.literal && (!name.given && name.family && name.isInstitution)) {
        name.literal = name.family;
    }
    if (name.literal) {
        delete name.family;
        delete name.given;
    }
    name = this._normalizeNameInput(name);
    var usedOrig;
    if (stopOrig) {
        usedOrig = stopOrig;
    } else {
        usedOrig = !foundTag;
    }
    return {name:name,usedOrig:usedOrig};
}

CSL.NameOutput.prototype.fixupInstitution = function (name, varname, listpos) {

    name = this._splitInstitution(name, varname, listpos);
    // XXX This should be embedded in the institution name function.
    if (this.institution.strings["reverse-order"]) {
        name["long"].reverse();
    }
        
    var long_form = name["long"];
    var short_form = long_form.slice();
    if (this.state.sys.getAbbreviation) {
        var jurisdiction = this.Item.jurisdiction;
        for (var j = 0, jlen = long_form.length; j < jlen; j += 1) {
            var jurisdiction = this.state.transform.loadAbbreviation(jurisdiction, "institution-part", long_form[j]);
            if (this.state.transform.abbrevs[jurisdiction]["institution-part"][long_form[j]]) {
                short_form[j] = this.state.transform.abbrevs[jurisdiction]["institution-part"][long_form[j]];
            }
        }
    }
    name["short"] = short_form;
    return name;
}


CSL.NameOutput.prototype.getStaticOrder = function (name, refresh) {
    var static_ordering_val = false;
    if (!refresh && name["static-ordering"]) {
        static_ordering_val = true;
    } else if (!(name.family.replace('"', '', 'g') + name.given).match(CSL.ROMANESQUE_REGEXP)) {
        static_ordering_val = true;
    } else if (name.multi && name.multi.main && name.multi.main.slice(0,2) == 'vn') {
        static_ordering_val = true;
    } else {
        if (this.state.opt['auto-vietnamese-names']
            && (CSL.VIETNAMESE_NAMES.exec(name.family + " " + name.given)
                && CSL.VIETNAMESE_SPECIALS.exec(name.family + name.given))) {
            
            static_ordering_val = true;
        }
    }
    return static_ordering_val;
}

CSL.NameOutput.prototype._splitInstitution = function (value, v, i) {
    var ret = {};
    var splitInstitution = value.literal.replace(/\s*\|\s*/g, "|");
    // check for total and utter abbreviation IFF form="short"
    splitInstitution = splitInstitution.split("|");
    if (this.institution.strings.form === "short" && this.state.sys.getAbbreviation) {
        // End processing before processing last single element, since
        // that will be picked up by normal element selection and
        // short-forming.
        var jurisdiction = this.Item.jurisdiction;
        for (var j = splitInstitution.length; j > 1; j += -1) {
            var str = splitInstitution.slice(0, j).join("|");
            var jurisdiction = this.state.transform.loadAbbreviation(jurisdiction, "institution-entire", str);
            if (this.state.transform.abbrevs[jurisdiction]["institution-entire"][str]) {
                var splitLst = this.state.transform.abbrevs[jurisdiction]["institution-entire"][str];
                var splitLst = splitLst.replace(/\s*\|\s*/g, "|");
                var splitLst = splitLst.split("|");
                splitInstitution = splitLst.concat(splitInstitution.slice(j));
            }
        }
    }
    splitInstitution.reverse();
    ret["long"] = this._trimInstitution(splitInstitution, v, i);
    return ret;
};

CSL.NameOutput.prototype._trimInstitution = function (subunits, v, i) {
	// 
    var use_first = false;
    var append_last = false;
    var stop_last = false;
    var s = subunits.slice();
    if (this.institution) {
        if ("undefined" !== typeof this.institution.strings["use-first"]) {
            use_first = this.institution.strings["use-first"];
        }
        if ("undefined" !== typeof this.institution.strings["stop-last"]) {
            // stop-last is negative when present
            s = s.slice(0, this.institution.strings["stop-last"]);
            subunits = subunits.slice(0, this.institution.strings["stop-last"]);
        }
        if ("undefined" !== typeof this.institution.strings["use-last"]) {
            append_last = this.institution.strings["use-last"];
        }
    }
    if (false === use_first) {
        if (this.persons[v].length === 0) {
            use_first = this.institution.strings["substitute-use-first"];
        }
        if (!use_first) {
            use_first = 0;
        }
    }
    if (false === append_last) {
        if (!use_first) {
            append_last = subunits.length;
        } else {
            append_last = 0;
        }
    }
    // Now that we've determined the value of append_last
    // (use-last), avoid overlaps.
    if (use_first > subunits.length - append_last) {
        use_first = subunits.length - append_last;
    }
    if (stop_last) {
        append_last = 0;
    }
    // This could be more clear. use-last takes priority
    // in the event of overlap, because of adjustment above
    subunits = subunits.slice(0, use_first);
    s = s.slice(use_first);
    if (append_last) {
        if (append_last > s.length) {
            append_last = s.length;
        }
        if (append_last) {
            subunits = subunits.concat(s.slice((s.length - append_last)));
        }
    }
    return subunits;
};
