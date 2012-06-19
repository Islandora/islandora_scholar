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

// Disambiguate names (the number of names is controlled externally, by successive
// runs of the processor).

/*global CSL: true */

CSL.NameOutput.prototype.disambigNames = function () {
    var pos = this.nameset_base;
    for (var i = 0, ilen = this.variables.length; i < ilen; i += 1) {
        var v = this.variables[i];
        if (this.freeters[v].length) {
            this._runDisambigNames(this.freeters[v], pos);
            this.state.tmp.disambig_settings.givens.push([]);
            pos += 1;
        }
        // We're skipping institutions, so another +1
        if (this.institutions[v].length) {
            this.state.tmp.disambig_settings.givens.push([]);
            pos += 1;
        }
        for (var j = 0, jlen = this.persons[v].length; j < jlen; j += 1) {
            if (this.persons[v][j].length) {
                this._runDisambigNames(this.persons[v][j], pos);
                this.state.tmp.disambig_settings.givens.push([]);
                pos += 1;
            }
        }
    }
};

CSL.NameOutput.prototype._runDisambigNames = function (lst, pos) {
    var chk, myform, myinitials, param, i, ilen, paramx;
    for (i = 0, ilen = lst.length; i < ilen; i += 1) {
        //
        // register the name in the global names disambiguation
        // registry
        this.state.registry.namereg.addname("" + this.Item.id, lst[i], i);
        chk = this.state.tmp.disambig_settings.givens[pos];
        if ("undefined" === typeof chk) {
            this.state.tmp.disambig_settings.givens.push([]);
        }
        chk = this.state.tmp.disambig_settings.givens[pos][i];
        if ("undefined" === typeof chk) {
            myform = this.name.strings.form;
            myinitials = this.name.strings["initialize-with"];
            param = this.state.registry.namereg.evalname("" + this.Item.id, lst[i], i, 0, myform, myinitials);
            this.state.tmp.disambig_settings.givens[pos].push(param);
        }
        //
        // set the display mode default for givennames if required
        myform = this.name.strings.form;
        myinitials = this.name.strings["initialize-with"];
        paramx = this.state.registry.namereg.evalname("" + this.Item.id, lst[i], i, 0, myform, myinitials);
        if (this.state.tmp.disambig_request) {
            //
            // fix a request for initials that makes no sense.
            // can't do this in disambig, because the availability
            // of initials is not a global parameter.
            var val = this.state.tmp.disambig_settings.givens[pos][i];
            // This is limited to by-cite disambiguation.
            if (val === 1 && 
                this.state.opt["givenname-disambiguation-rule"] === "by-cite" && 
                "undefined" === typeof this.name.strings["initialize-with"]) {
                val = 2;
            }
            param = val;
            if (this.state.opt["disambiguate-add-givenname"]) {
                param = this.state.registry.namereg.evalname("" + this.Item.id, lst[i], i, param, this.name.strings.form, this.name.strings["initialize-with"]);
            }
        } else {
            //
            // it clicks.  here is where we will put the
            // call to the names register, to get the floor value
            // for an individual name.
            //
            param = paramx;
        }
        // Need to save off the settings based on subsequent
        // form, when first cites are rendered.  Otherwise you
        // get full form names everywhere.
        if (!this.state.tmp.just_looking && this.item && this.item.position === CSL.POSITION_FIRST) {
            param = paramx;
        }
        if (!this.state.tmp.sort_key_flag) {
            this.state.tmp.disambig_settings.givens[pos][i] = param;
        }
    }
};
