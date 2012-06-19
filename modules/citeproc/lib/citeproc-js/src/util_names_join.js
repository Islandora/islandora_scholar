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

CSL.NameOutput.prototype.joinPersons = function (blobs, pos) {
    var ret;
    if (this.etal_spec[pos] === 1) {
        ret = this._joinEtAl(blobs, "name");
    } else if (this.etal_spec[pos] === 2) {
        ret = this._joinEllipsis(blobs, "name");
    } else if (!this.state.tmp.sort_key_flag) {
        ret = this._joinAnd(blobs, "name");
    } else {
        ret = this._join(blobs, " ");
    }
    return ret;
};


CSL.NameOutput.prototype.joinInstitutionSets = function (blobs, pos) {
    var ret;
    if (this.etal_spec[pos] === 1) {
        ret = this._joinEtAl(blobs, "institution");
    } else if (this.etal_spec[pos] === 2) {
        ret = this._joinEllipsis(blobs, "institution");
    } else {
        ret = this._joinAnd(blobs, "institution");
    }
    return ret;
};


CSL.NameOutput.prototype.joinPersonsAndInstitutions = function (blobs) {
    //
    return this._join(blobs, this.name.strings.delimiter);
};

CSL.NameOutput.prototype.joinFreetersAndInstitutionSets = function (blobs) {
    // Nothing, one or two, never more
    var ret = this._join(blobs, "[never here]", this["with"].single, this["with"].multiple);
    return ret;
};


CSL.NameOutput.prototype._joinEtAl = function (blobs, tokenname) {
    //
    var blob = this._join(blobs, this.name.strings.delimiter);
    
    // notSerious
    this.state.output.openLevel(this._getToken(tokenname));
    // Delimiter is applied from separately saved source in this case,
    // for discriminate application of single and multiple joins.
    this.state.output.current.value().strings.delimiter = "";
    this.state.output.append(blob, "literal", true);
    if (blobs.length > 1) {
        this.state.output.append(this["et-al"].multiple, "literal", true);
    } else if (blobs.length === 1) {
        this.state.output.append(this["et-al"].single, "literal", true);
    }
    this.state.output.closeLevel();
    return this.state.output.pop();
};


CSL.NameOutput.prototype._joinEllipsis = function (blobs, tokenname) {
    return this._join(blobs, this.name.strings.delimiter, this.name.ellipsis.single, this.name.ellipsis.multiple, tokenname);
};


CSL.NameOutput.prototype._joinAnd = function (blobs, tokenname) {
    return this._join(blobs, this[tokenname].strings.delimiter, this[tokenname].and.single, this[tokenname].and.multiple, tokenname);
};


CSL.NameOutput.prototype._join = function (blobs, delimiter, single, multiple, tokenname) {
    var i, ilen;
    if (!blobs) {
        return false;
    }
    // Eliminate false and empty blobs
    for (i = blobs.length - 1; i > -1; i += -1) {
        if (!blobs[i] || blobs[i].length === 0 || !blobs[i].blobs.length) {
            blobs = blobs.slice(0, i).concat(blobs.slice(i + 1));
        }
    }
    // XXXX This needs some attention before moving further.
    // Code is not sufficiently transparent.
    if (!blobs.length) {
        return false;
    } else if (single && blobs.length === 2) {
        blobs = [blobs[0], single, blobs[1]];
    } else {
        var delimiter_offset;
        if (multiple) {
            delimiter_offset = 2;
        } else {
            delimiter_offset = 1;
        }
        // It kind of makes sense down to here.
        for (i = 0, ilen = blobs.length - delimiter_offset; i < ilen; i += 1) {
            blobs[i].strings.suffix += delimiter;
        }
        if (blobs.length > 1) {
            var blob = blobs.pop();
            if (multiple) {
                blobs.push(multiple);
            } else {
                blobs.push(single);
            }
            blobs.push(blob);
        }
    }
    this.state.output.openLevel(this._getToken(tokenname));
    // Delimiter is applied from separately saved source in this case,
    // for discriminate application of single and multiple joins.
    if (single && multiple) {
        this.state.output.current.value().strings.delimiter = "";
    }
    for (i = 0, ilen = blobs.length; i < ilen; i += 1) {
        this.state.output.append(blobs[i], false, true);
    }
    this.state.output.closeLevel();
    return this.state.output.pop();
};


CSL.NameOutput.prototype._getToken = function (tokenname) {
    var token = this[tokenname];
    if (tokenname === "institution") {
        var newtoken = new CSL.Token();
        // Which, hmm, is the same thing as "empty"
        // Oh, well.
        //newtoken.strings.prefix = token.prefix;
        //newtoken.strings.suffix = token.suffix;
        return newtoken;
    }
    return token;
};
