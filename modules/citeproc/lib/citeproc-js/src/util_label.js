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

CSL.evaluateLabel = function (node, state, Item, item) {
    var myterm;
    if ("locator" === node.strings.term) {
        if (item && item.label) {
            if (item.label === "sub verbo") {
                myterm = "sub-verbo";
            } else {
                myterm = item.label;
            }
        }
        if (!myterm) {
            myterm = "page";
        }
    } else {
        myterm = node.strings.term;
    }
    // Plurals detection.
    var plural = 0;
    if ("locator" === node.strings.term) {
        if (item && item.locator) {
            if (state.opt.development_extensions.locator_parsing_for_plurals) {
                if (!state.tmp.shadow_numbers.locator) {
                    state.processNumber(false, item, "locator");
                }
                plural = state.tmp.shadow_numbers.locator.plural;
            } else {
                plural = CSL.evaluateStringPluralism(item.locator);
            }
        }
    } else if (["page", "page-first"].indexOf(node.variables[0]) > -1) {
        plural = CSL.evaluateStringPluralism(Item[myterm]);
    } else {
        if (!state.tmp.shadow_numbers[myterm]) {
            state.processNumber(false, Item, myterm);
        }
        plural = state.tmp.shadow_numbers[myterm].plural;
    }
/*
    if ("number" !== typeof plural) {
        if ("locator" == node.strings.term) {
            // check for plural flat field in supplementary item
            if (item) {
                plural = CSL.evaluateStringPluralism(item.locator);                
            }
        } else if (Item[node.strings.term]) {
            // check for plural flat field in main Item
            plural = CSL.evaluateStringPluralism(Item[node.strings.term]);            
        }
        // cleanup
        if ("number" !== typeof plural) {
            plural = 0;
        }
    }
*/
    return CSL.castLabel(state, node, myterm, plural);
};

CSL.evaluateStringPluralism = function (str) {
    if (str && str.match(/(?:[0-9],\s*[0-9]|\s+and\s+|&|[0-9]\s*[\-\u2013]\s*[0-9])/)) {
        return 1;
    } else {
        return 0;
    }
};

CSL.castLabel = function (state, node, term, plural, mode) {
    var ret = state.getTerm(term, node.strings.form, plural, false, mode);
    // XXXXX Cut-and-paste code in multiple locations. This code block should be
    // collected in a function.
    // Tag: strip-periods-block
    if (state.tmp.strip_periods) {
        ret = ret.replace(/\./g, "");
    } else {
        for (var i = 0, ilen = node.decorations.length; i < ilen; i += 1) {
            if ("@strip-periods" === node.decorations[i][0] && "true" === node.decorations[i][1]) {
                ret = ret.replace(/\./g, "");
                break;
            }
        }
    }
    return ret;
};
