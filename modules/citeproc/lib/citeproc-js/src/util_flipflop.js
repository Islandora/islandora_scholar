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

//
// (A) initialize flipflopper with an empty blob to receive output.
// Text string in existing output queue blob will be replaced with
// an array containing this blob.

CSL.Util.FlipFlopper = function (state) {
    var tagdefs, pos, len, p, entry, allTags, ret, def, esc, makeHashes, closeTags, flipTags, openToClose, openToDecorations, okReverse, hashes, allTagsLst, lst;
    this.state = state;
    this.blob = false;
    this.quotechars = ['"', "'"];
    tagdefs = [
        ["<i>", "</i>", "italics", "@font-style", ["italic", "normal","normal"], true],
        ["<b>", "</b>", "bold", "@font-weight", ["bold", "normal","normal"], true],
        ["<sup>", "</sup>", "superscript", "@vertical-align", ["sup", "sup","baseline"], true],
        ["<sub>", "</sub>", "subscript", "@vertical-align", ["sub", "sub","baseline"], true],
        ["<sc>", "</sc>", "smallcaps", "@font-variant", ["small-caps", "small-caps","normal"], true],
        ["<span class=\"nocase\">", "</span>", "passthrough", "@passthrough", ["true", "true","true"], true],
        ["<span class=\"nodecor\">", "</span>", "passthrough", "@passthrough", ["true", "true","true"], true],
        ['"',  '"',  "quotes",  "@quotes",  ["true",  "inner","true"],  "'"],
        [" '",  "'",  "quotes",  "@quotes",  ["inner",  "true","true"],  '"']
    ];
    //
    // plus quote defs from locale.
    //
    for (pos = 0; pos < 2; pos += 1) {
        p = ["-", "-inner-"][pos];
        entry = [];
        var openq = state.getTerm(("open" + p + "quote"));
        entry.push(openq);
        this.quotechars.push(openq);
        var closeq = state.getTerm(("close" + p + "quote"));
        entry.push(closeq);
        this.quotechars.push(closeq);
        entry.push(("quote" + "s"));
        entry.push(("@" + "quote" + "s"));
        if ("-" === p) {
            entry.push(["true", "inner"]);
        } else {
            entry.push(["inner", "true"]);
        }
        entry.push(true);
        if ("-" === p) {
            entry.push(state.getTerm(("close-inner-quote")));
        } else {
            entry.push(state.getTerm(("close-quote")));
        }
        tagdefs.push(entry);
    }
    allTags = function (tagdefs) {
        ret = [];
        len = tagdefs.length;
        for (pos = 0; pos < len; pos += 1) {
            def = tagdefs[pos];
            if (ret.indexOf(def[0]) === -1) {
                esc = "";
                if (["(", ")", "[", "]"].indexOf(def[0]) > -1) {
                    esc = "\\";
                }
                ret.push(esc + def[0]);
            }
            if (ret.indexOf(def[1]) === -1) {
                esc = "";
                if (["(", ")", "[", "]"].indexOf(def[1]) > -1) {
                    esc = "\\";
                }
                ret.push(esc + def[1]);
            }
        }
        return ret;
    };

    // This protects against empty quote defs in the locale,
    // which would otherwise cause the derived regexp to go
    // berserk and corrupt the string.
    allTagsLst = allTags(tagdefs);
    lst = [];
    for (pos = 0, len = allTagsLst.length; pos < len; pos += 1) {
        if (allTagsLst[pos]) {
            lst.push(allTagsLst[pos]);
        }
    }
    allTagsLst = lst.slice();
    this.allTagsRexMatch = new RegExp("(" + allTagsLst.join("|") + ")", "g");
    this.allTagsRexSplit = new RegExp("(?:" + allTagsLst.join("|") + ")");
    makeHashes = function (tagdefs) {
        closeTags = {};
        flipTags = {};
        openToClose = {};
        openToDecorations = {};
        okReverse = {};
        len = tagdefs.length;
        for (pos = 0; pos < len; pos += 1) {
            closeTags[tagdefs[pos][1]] = true;
            flipTags[tagdefs[pos][1]] = tagdefs[pos][5];
            openToClose[tagdefs[pos][0]] = tagdefs[pos][1];
            openToDecorations[tagdefs[pos][0]] = [tagdefs[pos][3], tagdefs[pos][4]];
            okReverse[tagdefs[pos][3]] = [tagdefs[pos][3], [tagdefs[pos][4][2], tagdefs[pos][1]]];
        }
        return [closeTags, flipTags, openToClose, openToDecorations, okReverse];
    };
    hashes = makeHashes(tagdefs);
    this.closeTagsHash = hashes[0];
    this.flipTagsHash = hashes[1];
    this.openToCloseHash = hashes[2];
    this.openToDecorations = hashes[3];
    this.okReverseHash = hashes[4];
};

CSL.Util.FlipFlopper.prototype.init = function (str, blob) {
    this.txt_esc = CSL.getSafeEscape(this.state.opt.mode, this.state.tmp.area);
    // CSL.debug("(flipflopper received blob decorations): "+blob.decorations);
    // CSL.debug("(blob alldecor): "+blob.alldecor);
    str = this._normalizeString(str);
    if (!blob) {
        this.strs = this.getSplitStrings(str);
        this.blob = new CSL.Blob();
    } else {
        this.blob = blob;
        this.strs = this.getSplitStrings(this.blob.blobs);
        this.blob.blobs = [];
    }
    this.blobstack = new CSL.Stack(this.blob);
    // CSL.debug("(this.blobstack.value() alldecor): "+this.blobstack.value().alldecor);
};

CSL.Util.FlipFlopper.prototype._normalizeString = function (str) {
    for (var i = 0, ilen = 2; i < ilen; i += 1) {
        str = str.replace(this.quotechars[i + 2], this.quotechars[0]);
        str = str.replace(this.quotechars[i + 4], this.quotechars[1]);
    }
    return str;
};

//
// (1) scan the string for escape characters.  Split the
// string on tag candidates, and rejoin the tags that
// are preceded by an escape character.  Ignore broken
// markup.
//
CSL.Util.FlipFlopper.prototype.getSplitStrings = function (str) {
    var strs, pos, len, newstr, head, tail, expected_closers, expected_openers, expected_flips, tagstack, badTagStack, posA, sameAsOpen, openRev, flipRev, tag, ibeenrunned, posB, wanted_closer, posC, sep, resplice, params, lenA, lenB, lenC, badTagPos, mx, myret;
    //
    // Do the split.
    //
    mx = str.match(this.allTagsRexMatch);
    strs = str.split(this.allTagsRexSplit);
    myret = [strs[0]];
    for (pos = 1, len = strs.length; pos < len; pos += 1) {
        myret.push(mx[pos - 1]);
        myret.push(strs[pos]);
    }
    strs = myret.slice();
    //
    // Resplice tags preceded by an escape character.
    //
    len = strs.length - 2;
    for (pos = len; pos > 0; pos += -2) {
        if (strs[(pos - 1)].slice((strs[(pos - 1)].length - 1)) === "\\") {
            newstr = strs[(pos - 1)].slice(0, (strs[(pos - 1)].length - 1)) + strs[pos] + strs[(pos + 1)];
            head = strs.slice(0, (pos - 1));
            tail = strs.slice((pos + 2));
            head.push(newstr);
            strs = head.concat(tail);
        }
    }
    //
    // Find tags that would break hierarchical symmetry.
    //
    expected_closers = [];
    expected_openers = [];
    expected_flips = [];
    tagstack = [];
    badTagStack = [];
    lenA = strs.length - 1;
    for (posA = 1; posA < lenA; posA += 2) {
        tag = strs[posA];
        if (this.closeTagsHash[tag]) {
            expected_closers.reverse();
            sameAsOpen = this.openToCloseHash[tag];
            openRev = expected_closers.indexOf(tag);
            flipRev = expected_flips.indexOf(tag);
            expected_closers.reverse();
            //
            // If I understand this correctly, we can cope with exactly
            // one level of nesting of inner and outer quotes inside
            // of the field.
            //
            if (!sameAsOpen || (openRev > -1 && (openRev < flipRev || flipRev === -1))) {
                ibeenrunned = false;
                lenB = expected_closers.length - 1;
                for (posB = lenB; posB > -1; posB += -1) {
                    ibeenrunned = true;
                    wanted_closer = expected_closers[posB];
                    if (tag === wanted_closer) {
                        expected_closers.pop();
                        expected_openers.pop();
                        expected_flips.pop();
                        tagstack.pop();
                        break;
                    }
                    // CSL.debug("badA:"+posA);
                    badTagStack.push(posA);
                }
                if (!ibeenrunned) {
                    // CSL.debug("badB:"+posA);
                    badTagStack.push(posA);
                }
                continue;
            }
        }
        if (this.openToCloseHash[tag]) {
            expected_closers.push(this.openToCloseHash[tag]);
            expected_openers.push(tag);
            expected_flips.push(this.flipTagsHash[tag]);
            tagstack.push(posA);
        }
    }
    lenC = expected_closers.length - 1;
    for (posC = lenC; posC > -1; posC += -1) {
        expected_closers.pop();
        expected_flips.pop();
        expected_openers.pop();
        // CSL.debug("badC:"+tagstack[(tagstack.length-1)]);
        badTagStack.push(tagstack.pop());
    }
    //
    // Resplice tags.
    //
    // Default sort algoritm in JS appears to be some string-based thing.  Go figure.
    badTagStack.sort(
        function (a, b) {
            if (a < b) {
                return 1;
            } else if (a > b) {
                return -1;
            }
            return 0;
        }
    );
    len = badTagStack.length;
    for (pos = 0; pos < len; pos += 1) {
        badTagPos = badTagStack[pos];
        head = strs.slice(0, (badTagPos - 1));
        tail = strs.slice((badTagPos + 2));
        sep = strs[badTagPos];
        //CSL.debug("sep [1] is: ("+sep+") for badTagPos: ("+badTagPos+") in strs ("+strs+")");
        if (sep.length && sep[0] !== "<" && this.openToDecorations[sep] && this.quotechars.indexOf(sep.replace(/\s+/g,"")) === -1) {
            params = this.openToDecorations[sep];
            sep = this.state.fun.decorate[params[0]][params[1][0]](this.state);
            //CSL.debug("sep [2] is: ("+sep+") from params[0] ("+params[0]+") and params[1][0] ("+params[1][0]+") -- params is ("+params+")");
        }
        resplice = strs[(badTagPos - 1)] + sep + strs[(badTagPos + 1)];
        head.push(resplice);
        strs = head.concat(tail);
    }

    len = strs.length;
    for (pos = 0; pos < len; pos += 2) {
        strs[pos] = strs[pos].replace("'", "\u2019", "g");
        strs[pos] = this.txt_esc(strs[pos]);
    }
    // XXXZ FIXME (done): swap punctuation for locators
    return strs;
};
//
// (2) scan the string for non-overlapping open and close tags,
// skipping escaped tags.  During processing, a list of expected
// closing tags will be maintained on a working stack.
//
CSL.Util.FlipFlopper.prototype.processTags = function () {
    var expected_closers, expected_openers, expected_flips, expected_rendering, str, posA, tag, prestr, newblob, blob, sameAsOpen, openRev, flipRev, posB, wanted_closer, newblobnest, param, fulldecor, level, decor, lenA, lenB, posC, lenC;

    expected_closers = [];
    expected_openers = [];
    expected_flips = [];
    expected_rendering = [];
    str = "";

    if (this.strs.length === 1) {
        this.blob.blobs = this.strs[0];
    } else if (this.strs.length > 2) {
        lenA = (this.strs.length - 1);
        for (posA = 1; posA < lenA; posA += 2) {
            tag = this.strs[posA];
            prestr = this.strs[(posA - 1)];
            // start by pushing in the trailing text string
            if (prestr) {
                newblob = new CSL.Blob(prestr);
                blob = this.blobstack.value();
                blob.push(newblob);
            }
//
// (a) For closing tags, check to see if it matches something
// on the working stack.  If so, pop the stack and close the
// output queue level.
//
            if (this.closeTagsHash[tag]) {
                //
                // Gaack.  Conditions.  Allow if ...
                // ... the close tag is not also an open tag, or ...
                // ... ... there is a possible open tag on our stacks, and ...
                // ... ... there is no intervening flipped partner to it.
                //
                expected_closers.reverse();
                sameAsOpen = this.openToCloseHash[tag];
                openRev = expected_closers.indexOf(tag);
                flipRev = expected_flips.indexOf(tag);
                expected_closers.reverse();

                if (!sameAsOpen || (openRev > -1 && (openRev < flipRev || flipRev === -1))) {
                    lenB = expected_closers.length;
                    for (posB = lenB; posB > -1; posB += -1) {
                        wanted_closer = expected_closers[posB];
                        if (tag === wanted_closer) {
                            expected_closers.pop();
                            expected_openers.pop();
                            expected_flips.pop();
                            expected_rendering.pop();
                            this.blobstack.pop();
                            break;
                        }
                    }
                    continue;
                }
            }
//
// (b) For open tags, push the corresponding close tag onto a working
// stack, and open a level on the output queue.
//
            if (this.openToCloseHash[tag]) {
                // CSL.debug("open:"+tag);
                expected_closers.push(this.openToCloseHash[tag]);
                expected_openers.push(tag);
                expected_flips.push(this.flipTagsHash[tag]);
                blob = this.blobstack.value();
                newblobnest = new CSL.Blob();
                blob.push(newblobnest);
                param = this.addFlipFlop(newblobnest, this.openToDecorations[tag]);
                //
                // No.  This can just impose the reverse of all normal decorations.
                //
                // CSL.debug(this.okReverseTagsHash[this.blob.alldecor[0][0].join("-is-")]);
                //
                if (tag === "<span class=\"nodecor\">") {
                    fulldecor = this.state[this.state.tmp.area].opt.topdecor.concat(this.blob.alldecor).concat([[["@quotes", "inner"]]]);

                    lenB = fulldecor.length;
                    for (posB = 0; posB < lenB; posB += 1) {
                        level = fulldecor[posB];
                        lenC = level.length;
                        for (posC = 0; posC < lenC; posC += 1) {
                            decor = level[posC];
                            if (["@font-style", "@font-weight", "@font-variant"].indexOf(decor[0]) > -1) {
                                // This is be the @name of the decor, plus a
                                // pairing composed of two copies of the "undo" side
                                // of the decor's format parameter.  The effect
                                // is to undo all decor at the top level of
                                // an <span class="nocase"> span.
                                param = this.addFlipFlop(newblobnest, this.okReverseHash[decor[0]]);
                            }
                        }
                    }
                }
                expected_rendering.push(this.state.fun.decorate[param[0]][param[1]](this.state));
                this.blobstack.push(newblobnest);
            }
        }
//
// (B) at the end of processing, unwind any open tags, append any
// remaining text to the output queue and return the blob.
//
        if (this.strs.length > 2) {
            str = this.strs[(this.strs.length - 1)];
            if (str) {
                blob = this.blobstack.value();
                newblob = new CSL.Blob(str);
                blob.push(newblob);
            }
        }
    }
    return this.blob;
};

CSL.Util.FlipFlopper.prototype.addFlipFlop = function (blob, fun) {
    var posA, posB, fulldecor, lenA, decorations, breakme, decor, posC, newdecor, lenC;
    posB = 0;
    fulldecor = this.state[this.state.tmp.area].opt.topdecor.concat(blob.alldecor).concat([[["@quotes", "inner"]]]);
    //var l = blob.alldecor.length;
    lenA = fulldecor.length;
    for (posA = 0; posA < lenA; posA += 1) {
        decorations = fulldecor[posA];
        breakme = false;
        lenC = decorations.length - 1;
        for (posC = lenC; posC > -1; posC += -1) {
            decor = decorations[posC];
            if (decor[0] === fun[0]) {
                if (decor[1] === fun[1][0]) {
                    posB = 1;
                }
                breakme = true;
                break;
            }
        }
        if (breakme) {
            break;
        }
    }
    newdecor = [fun[0], fun[1][posB]];
    blob.decorations.reverse();
    blob.decorations.push(newdecor);
    blob.decorations.reverse();
    return newdecor;
};
