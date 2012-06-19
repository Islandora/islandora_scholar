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

CSL.tokenExec = function (token, Item, item) {
    var next, maybenext, exec, pos, len, debug;
    debug = false;
    next = token.next;
    maybenext = false;
    //SNIP-START
    if (debug) {
        CSL.debug("---> Token: " + token.name + " (" + token.tokentype + ") in " + this.tmp.area + ", " + this.output.current.mystack.length);
    }
    //print("---> Token: " + token.name + " (" + token.tokentype + ") in " + this.tmp.area + ", " + this.output.current.mystack.length);
    //SNIP-END

    if (token.evaluator) {
        next = token.evaluator(token, this, Item, item);
    }
    len = token.execs.length;
    for (pos = 0; pos < len; pos += 1) {
        exec = token.execs[pos];
        maybenext = exec.call(token, this, Item, item);
        if (maybenext) {
            next = maybenext;
        }
    }
    //SNIP-START
    if (false) {
        CSL.debug(token.name + " (" + token.tokentype + ") ---> done");
    }
    //SNIP-END
    return next;
};

/**
 * Macro expander.
 * <p>Called on the state object.</p>
 */
CSL.expandMacro = function (macro_key_token) {
    var mkey, start_token, key, end_token, navi, macro_nodes, newoutput, mergeoutput, end_of_macro, func;

    mkey = macro_key_token.postponed_macro;
    if (this.build.macro_stack.indexOf(mkey) > -1) {
        throw "CSL processor error: call to macro \"" + mkey + "\" would cause an infinite loop";
    } else {
        this.build.macro_stack.push(mkey);
    }

    //
    // Here's where things change pretty dramatically.  We pull
    // macros out of E4X directly, and process them using the
    // same combination of tree walker and tag processor that
    // led us here, but with a different queue.
    //
    // Xml: get list of nodes by attribute match
    //
    var hasDate = false;
    macro_nodes = this.sys.xml.getNodesByName(this.cslXml, 'macro', mkey);
    if (macro_nodes.length) {
        hasDate = this.sys.xml.getAttributeValue(macro_nodes[0], "macro-has-date");
    }
    if (hasDate) {
        func = function (state, Item) {
            if (state.tmp.extension) {
                state.tmp["doing-macro-with-date"] = true;
            }
        };
        macro_key_token.execs.push(func);
    }
    //
    //
    // (true as the last argument suppresses quashing)
    macro_key_token.tokentype = CSL.START;
    CSL.Node.group.build.call(macro_key_token, this, this[this.build.area].tokens, true);

    //
    // Xml: test for node existence
    //
    if (!this.sys.xml.getNodeValue(macro_nodes)) {
        throw "CSL style error: undefined macro \"" + mkey + "\"";
    }
    navi = new this.getNavi(this, macro_nodes);
    CSL.buildStyle.call(this, navi);

    //
    // can't use the same token for the end of the group, since
    // both would then share the same jump-points, causing an
    // infinite loop.
    // (true as the last argument suppresses quashing)
    end_of_macro = new CSL.Token("group", CSL.END);
	if (macro_key_token.decorations) {
		end_of_macro.decorations = macro_key_token.decorations.slice();
    }

    if (hasDate) {
        func = function (state, Item) {
            if (state.tmp.extension) {
                state.tmp["doing-macro-with-date"] = false;
            }
        };
        end_of_macro.execs.push(func);
    }
    CSL.Node.group.build.call(end_of_macro, this, this[this.build.area].tokens, true);

    this.build.macro_stack.pop();

};



/**
 * Convert XML node to token.
 * <p>This is called on an XML node.  After extracting the name and attribute
 * information from the node, it performs three operations.  Attribute information
 * relating to output formatting is stored on the node as an array of tuples,
 * which fixes the sequence of execution of output functions to be invoked
 * in the next phase of processing.  Other attribute information is reduced
 * to functions, and is pushed into an array on the token in no particular
 * order, for later execution.  The element name is used as a key to
 * invoke the relevant <code>build</code> method of the target element.
 * Element methods are defined in {@link CSL.Node}.</p>
 * @param {Object} state  The state object returned by {@link CSL.Engine}.
 * @param {Int} tokentype  A CSL namespace constant (<code>CSL.START</code>,
 * <code>CSL.END</code> or <code>CSL.SINGLETON</code>.
 */
CSL.XmlToToken = function (state, tokentype) {
    var name, txt, attrfuncs, attributes, decorations, token, key, target;
    name = state.sys.xml.nodename(this);
    // CSL.debug(tokentype + " : " + name);
    if (state.build.skip && state.build.skip !== name) {
        return;
    }
    if (!name) {
        txt = state.sys.xml.content(this);
        if (txt) {
            state.build.text = txt;
        }
        return;
    }
    if (!CSL.Node[state.sys.xml.nodename(this)]) {
        throw "Undefined node name \"" + name + "\".";
    }
    attrfuncs = [];
    attributes = state.sys.xml.attributes(this);
    decorations = CSL.setDecorations.call(this, state, attributes);
    token = new CSL.Token(name, tokentype);
    if (tokentype !== CSL.END || name === "if" || name === "else-if" || name === "layout") {
        //
        // xml: more xml stuff
        //
        for (key in attributes) {
            if (attributes.hasOwnProperty(key)) {
                if (tokentype === CSL.END && key !== "@language" && key !== "@locale") {
                    continue;
                }
                if (attributes.hasOwnProperty(key)) {
                    try {
                        CSL.Attributes[key].call(token, state, "" + attributes[key]);
                    } catch (e) {
                        if (e === "TypeError: Cannot call method \"call\" of undefined") {
                            throw "Unknown attribute \"" + key + "\" in node \"" + name + "\" while processing CSL file";
                        } else {
                            throw "CSL processor error, " + key + " attribute: " + e;
                        }
                    }
                }
            }
        }
        token.decorations = decorations;
    }
    //
    // !!!!!: eliminate diversion of tokens to separate
    // token list (formerly used for reading in macros
    // and terms).
    //
    target = state[state.build.area].tokens;
    CSL.Node[name].build.call(token, state, target);
};


