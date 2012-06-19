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

CSL.Output = {};
/**
 * Output queue object.
 * @class
 */
CSL.Output.Queue = function (state) {
    this.levelname = ["top"];
    this.state = state;
    this.queue = [];
    this.empty = new CSL.Token("empty");
    var tokenstore = {};
    tokenstore.empty = this.empty;
    this.formats = new CSL.Stack(tokenstore);
    this.current = new CSL.Stack(this.queue);
};

// XXX This works, but causes a mismatch in api_cite
// Could insert a placeholder
// Better to have a function that spits out an independent blob.
// Is that possible though?
// Okay. Use queue.append() with fake_queue instead.
CSL.Output.Queue.prototype.pop = function () {
    // For some reason, state.output.current.value() here can be an array, 
    // not a blob ... ?
    var drip = this.current.value();
    if (drip.length) {
        return drip.pop();
    } else {
        return drip.blobs.pop();
    }
};

CSL.Output.Queue.prototype.getToken = function (name) {
    //SNIP-START
    CSL.debug("XXX loc [1]");
    //SNIP-END
    var ret = this.formats.value()[name];
    return ret;
};

CSL.Output.Queue.prototype.mergeTokenStrings = function (base, modifier) {
    var base_token, modifier_token, ret, key;
    //SNIP-START
    CSL.debug("XXX loc [2]");    
    //SNIP-END
    base_token = this.formats.value()[base];
    //SNIP-START
    CSL.debug("XXX loc [3]");
    //SNIP-END
    modifier_token = this.formats.value()[modifier];
    ret = base_token;
    if (modifier_token) {
        if (!base_token) {
            base_token = new CSL.Token(base, CSL.SINGLETON);
            base_token.decorations = [];
        }
        ret = new CSL.Token(base, CSL.SINGLETON);
        key = "";
        for (key in base_token.strings) {
            if (base_token.strings.hasOwnProperty(key)) {
                ret.strings[key] = base_token.strings[key];
            }
        }
        for (key in modifier_token.strings) {
            if (modifier_token.strings.hasOwnProperty(key)) {
                ret.strings[key] = modifier_token.strings[key];
            }
        }
        ret.decorations = base_token.decorations.concat(modifier_token.decorations);
    }
    return ret;
};

// Store a new output format token based on another
CSL.Output.Queue.prototype.addToken = function (name, modifier, token) {
    var newtok, attr;
    newtok = new CSL.Token("output");
    if ("string" === typeof token) {
    //SNIP-START
    CSL.debug("XXX loc [4]");
    //SNIP-END
        token = this.formats.value()[token];
    }
    if (token && token.strings) {
        for (attr in token.strings) {
            if (token.strings.hasOwnProperty(attr)) {
                newtok.strings[attr] = token.strings[attr];
            }
        }
        newtok.decorations = token.decorations;

    }
    if ("string" === typeof modifier) {
        newtok.strings.delimiter = modifier;
    }
    //SNIP-START
    CSL.debug("XXX loc [5]");
    //SNIP-END
    this.formats.value()[name] = newtok;
};

//
// newFormat adds a new bundle of formatting tokens to
// the queue's internal stack of such bundles
CSL.Output.Queue.prototype.pushFormats = function (tokenstore) {
    if (!tokenstore) {
        tokenstore = {};
    }
    //SNIP-START
    CSL.debug("XXX pushFormats()");
    //SNIP-END
    tokenstore.empty = this.empty;
    this.formats.push(tokenstore);
};


CSL.Output.Queue.prototype.popFormats = function (tokenstore) {
    //SNIP-START
    CSL.debug("XXX popFormats()");
    //SNIP-END
    this.formats.pop();
};

CSL.Output.Queue.prototype.startTag = function (name, token) {
    var tokenstore = {};
    if (this.state.tmp["doing-macro-with-date"] && this.state.tmp.extension) {
        token = this.empty;
        name = "empty";
    }
    tokenstore[name] = token;
    this.pushFormats(tokenstore);
    this.openLevel(name);
};

CSL.Output.Queue.prototype.endTag = function (name) {
    this.closeLevel();
    this.popFormats();
};

//
// newlevel adds a new blob object to the end of the current
// list, and adjusts the current pointer so that subsequent
// appends are made to blob list of the new object.

CSL.Output.Queue.prototype.openLevel = function (token, ephemeral) {
    var blob, curr, x, has_ephemeral;
    if ("object" === typeof token) {
        // delimiter, prefix, suffix, decorations from token
        blob = new CSL.Blob(undefined, token);
    } else if ("undefined" === typeof token) {
    //SNIP-START
    CSL.debug("XXX loc [6]");
    //SNIP-END
        blob = new CSL.Blob(undefined, this.formats.value().empty, "empty");
    } else {
        //SNIP-START
        CSL.debug("XXX loc [7]");
        //SNIP-END
        if (!this.formats.value() || !this.formats.value()[token]) {
            throw "CSL processor error: call to nonexistent format token \"" + token + "\"";
        }
        // delimiter, prefix, suffix, decorations from token
    //SNIP-START
    CSL.debug("XXX loc [8]");
    //SNIP-END
        blob = new CSL.Blob(undefined, this.formats.value()[token], token);
    }
    curr = this.current.value();
    curr.push(blob);
    this.current.push(blob);
};

/**
 * "merge" used to be real complicated, now it's real simple.
 */
CSL.Output.Queue.prototype.closeLevel = function (name) {
    // CLEANUP: Okay, so this.current.value() holds the blob at the
    // end of the current list.  This is wrong.  It should
    // be the parent, so that we have  the choice of reading
    // the affixes and decorations, or appending to its
    // content.  The code that manipulates blobs will be
    // much simpler that way.
    if (name && name !== this.current.value().levelname) {
        CSL.error("Level mismatch error:  wanted " + name + " but found " + this.current.value().levelname);
    }
    this.current.pop();
};

//
// append does the same thing as newlevel, except
// that the blob it pushes has text content,
// and the current pointer is not moved after the push.

CSL.Output.Queue.prototype.append = function (str, tokname, notSerious) {
    var token, blob, curr;
    var useblob = true;
    // XXXXX Nasty workaround, but still an improvement
    // over the reverse calls to the cs:date node build
    // function that we had before.
    if (this.state.tmp["doing-macro-with-date"]) {
        if (tokname !== "macro-with-date") {
            return false;
        }
        if (tokname === "macro-with-date") {
            tokname = "empty";
        }
    }
    if ("undefined" === typeof str) {
        return false;
    }
    if ("number" === typeof str) {
        str = "" + str;
    }
    if (!notSerious 
        && this.state.tmp.element_trace 
        && this.state.tmp.element_trace.value() === "suppress-me") {
        
        return false;
    }
    blob = false;
    if (!tokname) {
		//SNIP-START
		CSL.debug("XXX loc [9]");
		//SNIP-END
        token = this.formats.value().empty;
    } else if (tokname === "literal") {
        token = true;
        useblob = false;
    } else if ("string" === typeof tokname) {
		//SNIP-START
		CSL.debug("XXX loc [10]");
		//SNIP-END
        token = this.formats.value()[tokname];
    } else {
        token = tokname;
    }
    if (!token) {
        throw "CSL processor error: unknown format token name: " + tokname;
    }
    // Unset delimiters must be left undefined until they reach the queue
    // in order to discriminate unset from explicitly empty delimiters
    // when inheriting a default value from a superior node. [??? really ???]
    if (token.strings && "undefined" === typeof token.strings.delimiter) {
        token.strings.delimiter = "";
    }
    if ("string" === typeof str && str.length) {

        // Source (;?!»«): http://en.wikipedia.org/wiki/Space_(punctuation)#Breaking_and_non-breaking_spaces
        // Source (:): http://forums.zotero.org/discussion/4933/localized-quotes/#Comment_88384
        str = str.replace(/ ([:;?!\u00bb])/g, "\u202f$1").replace(/\u00ab /g, "\u00ab\u202f");

        this.last_char_rendered = str.slice(-1);
        // This, and not the str argument below on flipflop, is the
        // source of the flipflopper string source.
        str = str.replace(/\s+'/g, "  \'").replace(/^'/g, " \'");

        // signal whether we end with terminal punctuation?
        this.state.tmp.term_predecessor = true;
    }
    blob = new CSL.Blob(str, token);
    curr = this.current.value();
    if ("undefined" === typeof curr && this.current.mystack.length === 0) {
        // XXXX An operation like this is missing somewhere, this should NOT be necessary.
        // Addresses error triggered in multi-layouts.
        this.current.mystack.push([]);
        curr = this.current.value();
    }
    if ("string" === typeof blob.blobs) {
        if (this.state.tmp.strip_periods) {
            blob.blobs = blob.blobs.replace(/\./g, "");
        }
        this.state.tmp.term_predecessor = true;
    }
    //
    // Caution: The parallel detection machinery will blow up if tracking
    // variables are not properly initialized elsewhere.
    //
    if (!notSerious) {
        this.state.parallel.AppendBlobPointer(curr);
    }
    if ("string" === typeof str) {
        curr.push(blob);
        if (blob.strings["text-case"]) {
            //
            // This one is _particularly_ hard to follow.  It's not obvious,
            // but the blob already contains the input string at this
            // point, as blob.blobs -- it's a terminal node, as it were.
            // The str variable also contains the input string, but
            // that copy is not used for onward processing.  We have to
            // apply our changes to the blob copy.
            //
            blob.blobs = CSL.Output.Formatters[blob.strings["text-case"]](this.state, str);
        }
        //
        // XXX: Beware superfluous code in your code.  str in this
        // case is not the source of the final rendered string.
        // See note above.
        //
        this.state.fun.flipflopper.init(str, blob);
        //CSL.debug("(queue.append blob decorations): "+blob.decorations);
        this.state.fun.flipflopper.processTags();
    } else if (useblob) {
        curr.push(blob);
    } else {
        curr.push(str);
    }
    return true;
};

CSL.Output.Queue.prototype.string = function (state, myblobs, blob) {
    var i, ilen, j, jlen, b;
    //var blobs, ret, blob_delimiter, i, params, blobjr, last_str, last_char, b, use_suffix, qres, addtoret, span_split, j, res, blobs_start, blobs_end, key, pos, len, ppos, llen, ttype, ltype, terminal, leading, delimiters, use_prefix, txt_esc;
    var txt_esc = CSL.getSafeEscape(this.state.opt.mode, this.state.tmp.area);
    var blobs = myblobs.slice();
    var ret = [];
    
    if (blobs.length === 0) {
        return ret;
    }

    var blob_delimiter = "";
    if (blob) {
        blob_delimiter = blob.strings.delimiter;
    } else {
        //print("=== Setting false to start ===");
        state.tmp.count_offset_characters = false;
        state.tmp.offset_characters = 0;
    }

    if (blob && blob.new_locale) {
        blob.old_locale = state.opt.lang;
        state.opt.lang = blob.new_locale;
    }

    var blobjr, use_suffix, use_prefix, params;
    for (i = 0, ilen = blobs.length; i < ilen; i += 1) {
        blobjr = blobs[i];

        if (blobjr.strings.first_blob) {
            // Being the Item.id of the the entry being rendered.
            //print("  -- turning on counting");
            state.tmp.count_offset_characters = blobjr.strings.first_blob;
        }

        if ("string" === typeof blobjr.blobs) {
            if ("number" === typeof blobjr.num) {
                ret.push(blobjr);
            } else if (blobjr.blobs) {
                // (skips empty strings)
                //b = txt_esc(blobjr.blobs);
                b = blobjr.blobs;
                var blen = b.length;

                if (!state.tmp.suppress_decorations) {
                    for (j = 0, jlen = blobjr.decorations.length; j < jlen; j += 1) {
                        params = blobjr.decorations[j];
                        if (state.normalDecorIsOrphan(blobjr, params)) {
                            continue;
                        }
                        b = state.fun.decorate[params[0]][params[1]](state, b);
                    }
                }
                //
                // because we will rip out portions of the output
                // queue before rendering, group wrappers need
                // to produce no output if they are found to be
                // empty.
                if (b && b.length) {
                    b = txt_esc(blobjr.strings.prefix) + b + txt_esc(blobjr.strings.suffix);
                    ret.push(b);
                    if (state.tmp.count_offset_characters) {
                        state.tmp.offset_characters += (blen + blobjr.strings.suffix.length + blobjr.strings.prefix.length);
                    }
                }
            }
        } else if (blobjr.blobs.length) {
            var addtoret = state.output.string(state, blobjr.blobs, blobjr);
            // Apparently no longer used
            //if (ret.slice(-1)[0] && addtoret.slice(-1)[0]) {
            //    var ttype = typeof ret.slice(-1)[0];
            //    var ltype = typeof addtoret.slice(-1)[0];
            //    //
            //    // The list generated by the string function is a mixture
            //    // of strings and numeric data objects awaiting evaluation
            //    // for ranged joins.  If we hit one of them, we skip this
            //    // fixit operation.
            //    //
            //    if ("string" === ttype && "string" === ltype) {
            //        terminal = ret.slice(-1)[0].slice(-1);
            //        leading = addtoret.slice(-1)[0].slice(0, 1);
            //    }
            //}
            ret = ret.concat(addtoret);
        }
        if (blobjr.strings.first_blob) {
            // The Item.id of the entry being rendered.
            state.registry.registry[state.tmp.count_offset_characters].offset = state.tmp.offset_characters;
            state.tmp.count_offset_characters = false;
        }
    }
    var span_split = 0;
    for (i = 0, ilen = ret.length; i < ilen; i += 1) {
        if ("string" === typeof ret[i]) {
            span_split = (parseInt(i, 10) + 1);
            if (i < ret.length - 1  && "object" === typeof ret[i + 1]) {
                if (blob_delimiter && !ret[i + 1].UGLY_DELIMITER_SUPPRESS_HACK) {
                    //has_more = true;
                    ret[i] += txt_esc(blob_delimiter);
                } else {
                    //has_more = false;
                }
                // One bite of the apple
                ret[i + 1].UGLY_DELIMITER_SUPPRESS_HACK = true;
            }
            //span_split = ret.length;
            //print("XXX ret: "+ret+" -- "+blob_delimiter);
        }
    }
/*

    // XXX NOT good here. What happens if we have boy-girl-goy-girl?
    var span_split = 0;
    var has_more = false;
    for (i = 0, ilen = ret.length; i < ilen; i += 1) {
        if ("string" === typeof ret[i]) {
            span_split = (parseInt(i, 10) + 1);
            if (i < ret.length - 1  && "object" === typeof ret[i + 1]) {
                if (!ret[i + 1].UGLY_DELIMITER_SUPPRESS_HACK) {
                    has_more = true;
                } else {
                    has_more = false;
                }
                // One bite of the apple
                ret[i + 1].UGLY_DELIMITER_SUPPRESS_HACK = true;
            }
            //span_split = ret.length;
            //print("XXX ret: "+ret+" -- "+blob_delimiter);
        }
    }
*/
    //if (span_split !== ret.length) {
    //  print("span_split: "+span_split+", ret.length: "+ret.length);
    //    print("   "+ret);
    //}
    if (blob && (blob.decorations.length || blob.strings.suffix || blob.strings.prefix)) {
        span_split = ret.length;
    }

    //var mytype = "string";
    //for (var q = 0, qlen = ret.length; q < qlen; q += 1) {
    //    if (typeof ret[q] !== "string") {
    //        mytype = typeof ret[q];
    //        break;
    //    }
    //}
    var blobs_start = state.output.renderBlobs(ret.slice(0, span_split), blob_delimiter);
    if (blobs_start && blob && (blob.decorations.length || blob.strings.suffix || blob.strings.prefix)) {
        if (!state.tmp.suppress_decorations) {
            for (i = 0, ilen = blob.decorations.length; i < ilen; i += 1) {
                params = blob.decorations[i];
                if (["@bibliography", "@display"].indexOf(params[0]) > -1) {
                    continue;
                }
                if (state.normalDecorIsOrphan(blobjr, params)) {
                    continue;
                }
                blobs_start = state.fun.decorate[params[0]][params[1]](state, blobs_start);
            }
        }
        //
        // XXXX: cut-and-paste warning.  same as a code block above.
        //
        b = blobs_start;
        use_suffix = blob.strings.suffix;
        if (b && b.length) {
            use_prefix = blob.strings.prefix;
            b = txt_esc(use_prefix) + b + txt_esc(use_suffix);
            if (state.tmp.count_offset_characters) {
                state.tmp.offset_characters += (use_prefix.length + use_suffix.length);
            }
        }
        blobs_start = b;
        if (!state.tmp.suppress_decorations) {
            for (i = 0, ilen = blob.decorations.length; i < ilen; i += 1) {
                params = blob.decorations[i];
                if (["@bibliography", "@display"].indexOf(params[0]) === -1) {
                    continue;
                }
                blobs_start = state.fun.decorate[params[0]][params[1]].call(blob, state, blobs_start);
            }
        }
    }
    var blobs_end = ret.slice(span_split, ret.length);
    if (!blobs_end.length && blobs_start) {
        ret = [blobs_start];
    } else if (blobs_end.length && !blobs_start) {
        ret = blobs_end;
    } else if (blobs_start && blobs_end.length) {
        ret = [blobs_start].concat(blobs_end);
    }
    //
    // Blobs is now definitely a string with
    // trailing blobs.  Return it.
    if ("undefined" === typeof blob) {
        this.queue = [];
        this.current.mystack = [];
        this.current.mystack.push(this.queue);
        if (state.tmp.suppress_decorations) {
            ret = state.output.renderBlobs(ret);
        }
    } else if ("boolean" === typeof blob) {
        ret = state.output.renderBlobs(ret);
    }

    if (blob && blob.new_locale) {
        state.opt.lang = blob.old_locale;
    }


    if (blob) {
        return ret;
    } else {
        return ret;
    }
};

CSL.Output.Queue.prototype.clearlevel = function () {
    var blob, pos, len;
    blob = this.current.value();
    len = blob.blobs.length;
    for (pos = 0; pos < len; pos += 1) {
        blob.blobs.pop();
    }
};

CSL.Output.Queue.prototype.renderBlobs = function (blobs, delim, has_more) {
    var state, ret, ret_last_char, use_delim, i, blob, pos, len, ppos, llen, pppos, lllen, res, str, params, txt_esc;
    txt_esc = CSL.getSafeEscape(this.state.opt.mode, this.state.tmp.area);
    if (!delim) {
        delim = "";
    }
    state = this.state;
    ret = "";
    ret_last_char = [];
    use_delim = "";
    len = blobs.length;
    for (pos = 0; pos < len; pos += 1) {
        if (blobs[pos].checkNext) {
            blobs[pos].checkNext(blobs[(pos + 1)]);
        }
    }
    // Fix last non-range join
    var doit = true;
    for (pos = blobs.length - 1; pos > 0; pos += -1) {
        if (blobs[pos].checkLast) {
        if (doit && blobs[pos].checkLast(blobs[pos - 1])) {
            doit = false;
        }
        } else {
        doit = true;
        }
    }
    len = blobs.length;
    for (pos = 0; pos < len; pos += 1) {
        blob = blobs[pos];
        if (ret) {
            use_delim = delim;
        }
        if (blob && "string" === typeof blob) {
            ret += txt_esc(use_delim);
            // XXX Blob should be run through flipflop and flattened here.
            // (I think it must be a fragment of text around a numeric
            // variable)
            ret += blob;
            if (state.tmp.count_offset_characters) {
                //state.tmp.offset_characters += (use_delim.length + blob.length);
                state.tmp.offset_characters += (use_delim.length);
            }
            //if (has_more && pos === len - 1) {
            //    ret += txt_esc(delim);            }
        } else if (blob.status !== CSL.SUPPRESS) {
            str = blob.formatter.format(blob.num, blob.gender);
            // Workaround to get a more or less accurate value.
            var strlen = str.replace(/<[^>]*>/g, "").length;
            // notSerious
            this.append(str, "empty", true);
            var str_blob = this.pop();
            var count_offset_characters = state.tmp.count_offset_characters;
            str = this.string(state, [str_blob], false);
            state.tmp.count_offset_characters = count_offset_characters;
            if (blob.strings["text-case"]) {
                str = CSL.Output.Formatters[blob.strings["text-case"]](this.state, str);
            }
            if (!state.tmp.suppress_decorations) {
                llen = blob.decorations.length;
                for (ppos = 0; ppos < llen; ppos += 1) {
                    params = blob.decorations[ppos];
                    if (state.normalDecorIsOrphan(blob, params)) {
                        continue;
                    }
                    str = state.fun.decorate[params[0]][params[1]](state, str);
                }
            }
            str = blob.strings.prefix + str + blob.strings.suffix;
            var addme = "";
            if (blob.status === CSL.END) {
                addme = txt_esc(blob.range_prefix);
            } else if (blob.status === CSL.SUCCESSOR) {
                addme = txt_esc(blob.successor_prefix);
            } else if (blob.status === CSL.START) {
                addme = "";
            } else if (blob.status === CSL.SEEN) {
                addme = txt_esc(blob.splice_prefix);
            }
            ret += addme;
            ret += str;
            if (state.tmp.count_offset_characters) {
                state.tmp.offset_characters += (addme.length + blob.strings.prefix.length + strlen + blob.strings.suffix.length);
            }
        }
    }
    return ret;
};

CSL.Output.Queue.purgeEmptyBlobs = function (myblobs, endOnly) {
    var res, i, ilen, j, jlen, tmpblobs;
    if ("string" === typeof myblobs || !myblobs.length) {
        return;
    }
    for (i = myblobs.length - 1; i > -1; i += -1) {
        CSL.Output.Queue.purgeEmptyBlobs(myblobs[i].blobs, endOnly);
    }
    for (i = myblobs.length - 1; i > -1; i += -1) {
        // Edit myblobs in place
        if (!myblobs[i].blobs.length) {
            tmpblobs = myblobs.slice(i + 1);
            for (j = i, jlen = myblobs.length; j < jlen; j += 1) {
                myblobs.pop();
            }
            for (j = 0, jlen = tmpblobs.length; j < jlen; j += 1) {
                myblobs.push(tmpblobs[j]);
            }
        }
        if (endOnly) {
            break;
        }
    }
};

// XXXXX: Okay, stop and think about the following two functions.
// Spaces have no formatting characteristics, so they can be
// safely purged at lower levels.  If a separate function is used
// for punctuation (i.e. the original setup), and special-purpose
// functions are applied to spaces, we can get more robust 
// behavior without breaking things all over the place.

CSL.Output.Queue.purgeNearsidePrefixChars = function(myblob, chr) {
    if (!chr) {
        return;
    }
    if ("object" === typeof myblob) {
        if ((CSL.TERMINAL_PUNCTUATION.indexOf(chr) > -1 && 
             CSL.TERMINAL_PUNCTUATION.slice(0, -1).indexOf(myblob.strings.prefix.slice(0, 1)) > -1)) {
            myblob.strings.prefix = myblob.strings.prefix.slice(1);
        } else if ("object" === typeof myblob.blobs) {
            CSL.Output.Queue.purgeNearsidePrefixChars(myblob.blobs[0], chr);
        }
    }
};

CSL.Output.Queue.purgeNearsidePrefixSpaces = function(myblob, chr) {
    //if (!chr) {
    //    return;
    //}
    if ("object" === typeof myblob) {
        if (" " === chr && " " === myblob.strings.prefix.slice(0, 1)) {
            myblob.strings.prefix = myblob.strings.prefix.slice(1);
        } else if ("object" === typeof myblob.blobs) {
            CSL.Output.Queue.purgeNearsidePrefixSpaces(myblob.blobs[0], chr);
        }
    }
};

CSL.Output.Queue.purgeNearsideSuffixSpaces = function(myblob, chr) {
    if ("object" === typeof myblob) {
        if (" " === chr && " " === myblob.strings.suffix.slice(-1)) {
            myblob.strings.suffix = myblob.strings.suffix.slice(0, -1);
        } else if ("object" === typeof myblob.blobs) {
            if (!chr) {
                chr = myblob.strings.suffix.slice(-1);
            }
            chr = CSL.Output.Queue.purgeNearsideSuffixSpaces(myblob.blobs[myblob.blobs.length - 1], chr);
        } else {
            chr = myblob.strings.suffix.slice(-1);
        }
    }
    return chr;
};

CSL.Output.Queue.adjustPunctuation = function (state, myblobs, stk, finish) {
    var chr, suffix, dpref, blob, delimiter, suffixX, dprefX, blobX, delimiterX, prefix, prefixX, dsuffX, dsuff, slast, dsufff, dsufffX, lastchr, firstchr, exposed_suffixes, exposed, j, jlen, i, ilen;

    var TERMS = CSL.TERMINAL_PUNCTUATION.slice(0, -1);
    var TERM_OR_SPACE = CSL.TERMINAL_PUNCTUATION;
    var SWAPS = CSL.SWAPPING_PUNCTUATION;
    
    if (!stk) {
        stk = [{suffix: "", delimiter: ""}];
    }

    slast = stk.length - 1;

    delimiter = stk[slast].delimiter;
    dpref = stk[slast].dpref;
    dsuff = stk[slast].dsuff;
    dsufff = stk[slast].dsufff;
    prefix = stk[slast].prefix;
    suffix = stk[slast].suffix;
    blob = stk[slast].blob;

    if ("string" === typeof myblobs) {
        // Note that (1) the "suffix" variable is set 
        // non-nil only if it contains terminal punctuation;
        // (2) "myblobs" is a string in this case; (3) we
        // don't try to control duplicate spaces, because
        // if they're in the user-supplied string somehow, 
        // they've been put there by intention.
        if (suffix) {
            if (blob && 
                TERMS.indexOf(myblobs.slice(-1)) > -1 &&
                TERMS.slice(1).indexOf(suffix) > -1 &&
                blob.strings.suffix !== " ") {
                    blob.strings.suffix = blob.strings.suffix.slice(1);
            }
        }
        lastchr = myblobs.slice(-1);
        firstchr = myblobs.slice(0,1);
    } else {
        // Complete the move of a leading terminal punctuation 
        // from superior delimiter to suffix at this level,
        // to allow selective suppression.
        if (dpref) {
            for (j = 0, jlen = myblobs.length - 1; j < jlen; j += 1) {
                var t = myblobs[j].strings.suffix.slice(-1);
                // print("hey: ["+j+"] ("+dpref+") ("+myblobs[0].blobs+")")

                if (TERMS.indexOf(t) === -1 ||
                    TERMS.indexOf(dpref) === -1) {
                    // Drop duplicate space
                    if (dpref !== " " || dpref !== myblobs[j].strings.suffix.slice(-1)) {
                        myblobs[j].strings.suffix += dpref;                        
                    }
                }
            }
        }

        // For ParentalSuffixPrefixUphill
        if (suffix === " ") {
            CSL.Output.Queue.purgeNearsideSuffixSpaces(myblobs[myblobs.length - 1], " ");
        }
        var lst = [];
        var doblob;
        for (i = 0, ilen = myblobs.length - 1; i < ilen; i += 1) {
            doblob = myblobs[i];
            var following_prefix = myblobs[i + 1].strings.prefix;
            chr = false;
            // A record of the suffix leading character
            // nearest to each empty delimiter, for use
            // in comparisons in the next function.
            var ret = CSL.Output.Queue.purgeNearsideSuffixSpaces(doblob, chr);
            if (!dsuff) {
                lst.push(ret);
            } else {
                lst.push(false);
            }
        }

        // For ParentalSuffixPrefixDownhill
        chr = false;
        for (i = 1, ilen = myblobs.length; i < ilen; i += 1) {
            doblob = myblobs[i];
            chr = "";
            var preceding_suffix = myblobs[i - 1].strings.suffix;
            if (dsuff === " ") {
                chr = dsuff;
            } else if (preceding_suffix) {
                chr = preceding_suffix.slice(-1);
            } else if (lst[i - 1]) {
                chr = lst[i - 1];
            }
            CSL.Output.Queue.purgeNearsidePrefixSpaces(doblob, chr);
        }
        if (dsufff) {
            CSL.Output.Queue.purgeNearsidePrefixSpaces(myblobs[0], " ");
        } else if (prefix === " ") {
            CSL.Output.Queue.purgeNearsidePrefixSpaces(myblobs[0], " ");
        }

        // Descend down the nearest blobs until we run into
        // a string blob or a prefix, and if we find a
        // prefix with an initial character that conflicts
        // with the lastchr found so far, quash the prefix char.
        for (i = 0, ilen = myblobs.length; i < ilen; i += 1) {
            doblob = myblobs[i];

            CSL.Output.Queue.purgeNearsidePrefixChars(doblob, lastchr);
            
            // Prefix and suffix
            if (i === 0) {
                if (prefix) {
                    if (doblob.strings.prefix.slice(0, 1) === " ") {
                        //doblob.strings.prefix = doblob.strings.prefix.slice(1);
                    }
                }
            }
            
            if (dsufff) {
                if (doblob.strings.prefix) {
                    if (i === 0) {
                        if (doblob.strings.prefix.slice(0, 1) === " ") {
                            //doblob.strings.prefix = doblob.strings.prefix.slice(1);
                        }
                    }
                }
            }
            if (dsuff) {
                if (i > 0) {
                    if (doblob.strings.prefix.slice(0, 1) === " ") {
                        //doblob.strings.prefix = doblob.strings.prefix.slice(1);
                    }
                }
            }

            if (i < (myblobs.length - 1)) {
                // Migrate any leading terminal punctuation on a subsequent
                // prefix to the current suffix, iff the
                // (remainder of the) intervening delimiter is empty.
                // Needed for CSL of the Chicago styles.
                var nextprefix = myblobs[i + 1].strings.prefix;
                if (!delimiter) {
                    if (nextprefix) {
                        var nxtchr = nextprefix.slice(0, 1);
                        if (SWAPS.indexOf(nxtchr) > -1) {
                            myblobs[i + 1].strings.prefix = nextprefix.slice(1);
                            if (TERMS.indexOf(nxtchr) === -1 ||
                                (TERMS.indexOf(nxtchr) > -1 &&
                                 TERMS.indexOf(doblob.strings.suffix.slice(-1)) === -1)) {
                                     doblob.strings.suffix += nxtchr;
                            }
                        } else if (nxtchr === " " &&
                                    doblob.strings.suffix.slice(-1) === " ") {
                            doblob.strings.suffix = doblob.strings.suffix.slice(0, -1);
                        }
                    }
                }
            }

            // If duplicate punctuation on superior suffix,
            // quash on superior object.
            if (i === (myblobs.length - 1)) {
                if (suffix) {
                    if (doblob.strings.suffix && 
//                        (suffix === doblob.strings.suffix.slice(-1) ||
                        (TERMS.slice(1).indexOf(suffix) > -1 &&
                          TERMS.indexOf(doblob.strings.suffix.slice(-1)) > -1)) {
                            blob.strings.suffix = blob.strings.suffix.slice(1);
                    }
                }
            }

            // Swap punctuation into quotation marks as required.
            //if (i === (myblobs.length - 1) && state.getOpt('punctuation-in-quote')) {
            if (state.getOpt('punctuation-in-quote')) {
                var decorations = doblob.decorations;
                for (j = 0, jlen = decorations.length; j < jlen; j += 1) {
                    if (decorations[j][0] === '@quotes' && decorations[j][1] === 'true') {
                        var swapchar = doblob.strings.suffix.slice(0, 1);
                        var swapblob = false;
                        if (SWAPS.indexOf(swapchar) > -1) {
                            swapblob = doblob;
                        } else if (SWAPS.indexOf(suffix) > -1 && i === (myblobs.length - 1)) {
                            swapchar = suffix;
                            swapblob = blob;
                        } else {
                            swapchar = false;
                        }
                        // This reflects Chicago 16th.
                        if (swapchar) {
                            // For both possible case, if ending punctuation is 
                            // not in SWAPS, add the swapchar.
                            // Otherwise add the swapchar only if ending punctuation 
                            // is in TERMS, and the swapchar is in SWAPS and not in TERMS.
                            //
                            // Code could do with some pruning, but that's the logic of it.
                            if ("string" === typeof doblob.blobs) {
                                if (SWAPS.indexOf(doblob.blobs.slice(-1)) === -1 ||
                                   (TERMS.indexOf(doblob.blobs.slice(-1)) > -1 &&
                                    SWAPS.indexOf(swapchar) > -1 &&
                                    TERMS.indexOf(swapchar) === -1)) {
                                        doblob.blobs += swapchar;
                                }
                            } else {
                                if (SWAPS.indexOf(doblob.blobs.slice(-1)[0].strings.suffix.slice(-1)) === -1 ||
                                    (TERMS.indexOf(doblob.blobs.slice(-1)[0].strings.suffix.slice(-1)) > -1 &&
                                     SWAPS.indexOf(swapchar) > -1 &&
                                     TERMS.indexOf(swapchar) === -1)) {
                                         doblob.blobs.slice(-1)[0].strings.suffix += swapchar;
                                }
                            }
                            swapblob.strings.suffix = swapblob.strings.suffix.slice(1);
                        }
                    }
                }
            }

            // Prepare variables for the sniffing stack, for use
            // in the next recursion.

            if (i === (myblobs.length - 1)) {
                // If last blob in series, use superior suffix if current
                // level has none.
                if (doblob.strings.suffix) {
                    suffixX = doblob.strings.suffix.slice(0, 1);
                    blobX = doblob;
                } else {
                    suffixX = stk[stk.length - 1].suffix;
                    blobX = stk[stk.length - 1].blob;
                }
            } else {
                // If NOT last blob in series, use only the current
                // level suffix for sniffing.
                if (doblob.strings.suffix) {
                    suffixX = doblob.strings.suffix.slice(0, 1);
                    blobX = doblob;
                } else {
                    suffixX = "";
                    blobX = false;
                }
                
            }

            // Use leading suffix char for sniffing only if it
            // is a terminal punctuation character.
            if (SWAPS.concat([" "]).indexOf(suffixX) === -1) {
            //if (SWAPS.indexOf(suffixX) === -1) {
                suffixX = "";
                blobX = false;
            }

            // Use leading delimiter char for sniffing only if it
            // is a terminal punctuation character.
            if (doblob.strings.delimiter && 
                doblob.blobs.length > 1) {
                dprefX = doblob.strings.delimiter.slice(0, 1);
                if (SWAPS.concat([" "]).indexOf(dprefX) > -1) {
                //if (SWAPS.indexOf(dprefX) > -1) {
                    doblob.strings.delimiter = doblob.strings.delimiter.slice(1);
                } else {
                    dprefX = "";
                }
            } else {
                dprefX = "";
            }
            
            if (doblob.strings.prefix) {
                if (doblob.strings.prefix.slice(-1) === " ") {
                    // Marker copy only, no slice at this level before descending.
                    prefixX = " ";
                } else {
                    prefixX = "";
                }
            } else {
                if (i === 0) {
                    prefixX = prefix;                    
                } else {
                    prefixX = "";
                }
            }

            if (dsuff) {
                dsufffX = dsuff;
            } else {
                if (i === 0) {
                    dsufffX = dsufff;                    
                } else {
                    dsufffX = "";
                }
            }
            if (doblob.strings.delimiter) {
                if (doblob.strings.delimiter.slice(-1) === " " &&
                    "object" === typeof doblob.blobs && doblob.blobs.length > 1) {
                       dsuffX = doblob.strings.delimiter.slice(-1);
                } else {
                    dsuffX = "";                        
                }
            } else {
                dsuffX = "";                    
            }
            
            delimiterX = doblob.strings.delimiter;

            // Push variables to stack and recurse.
            stk.push({suffix: suffixX, dsuff:dsuffX, blob:blobX, delimiter:delimiterX, prefix:prefixX, dpref: dprefX, dsufff: dsufffX});
            lastchr = CSL.Output.Queue.adjustPunctuation(state, doblob.blobs, stk);
        }
        
        // cmd_cite.js needs a report of the last character to be
        // rendered, to suppress extraneous trailing periods in
        // rare cases.
        if (myblobs && myblobs.length) {
            var last_suffix = myblobs[myblobs.length - 1].strings.suffix;
            if (last_suffix) {
                lastchr = last_suffix.slice(-1);
            }
        }
    }
    // Always pop the stk when returning, unless it's the end of the line
    // (return value is needed in cmd_cite.js, so that the adjusted
    // suffix can be extracted from the fake blob used at top level).
    if (stk.length > 1) {
        stk.pop();
    }
    // Are these needed?
    state.tmp.last_chr = lastchr;
    return lastchr;
};

