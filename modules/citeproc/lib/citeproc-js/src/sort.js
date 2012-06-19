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

CSL.getSortCompare = function () {
    var strcmp;
    try {
        var Components;
        var localeService = Components.classes["@mozilla.org/intl/nslocaleservice;1"]
            .getService(Components.interfaces.nsILocaleService);
        var collationFactory = Components.classes["@mozilla.org/intl/collation-factory;1"]
            .getService(Components.interfaces.nsICollationFactory);
        var collation = collationFactory.CreateCollation(localeService.getApplicationLocale());
        strcmp = function(a, b) {
            return collation.compareString(1, a, b);
        };
        CSL.debug("Using collation sort");
    } catch (e) {
        strcmp = function (a, b) {
            return a.localeCompare(b);
        };
    }
    var isKana = /^[\u3040-\u309f\u30a0-\u30ff]/;
    var sortCompare = function (a, b) {
        // Display rows with empty values last
        //var cmp = (a == '' && b != '') ? -1 : (a != '' && b == '') ? 1 : 0;
        //if (cmp) {
        //    return cmp;
        //}
        var ak = isKana.exec(a);
        var bk = isKana.exec(b);
        if (ak || bk) {
            if (!ak) {
                return -1;
            } else if (!bk) {
                return 1;
            } else if (a < b) {
                return -1;
            } else if (a > b) {
                return 1;
            } else {
                return 0;
            }
        } else {
            return strcmp(a, b);
        }
    };
    return sortCompare;
};
