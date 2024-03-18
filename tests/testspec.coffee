import { describe } from 'coffeelib/mocha/lib/mocha'

import '../index'
jasmine = require('jasmine')


describe "Template", ->
    describe "#fromFile", ->
        return it "Reads from file", ->
            html_tmpl = Cacao.Environment.Template.fromFile '../examples/html.cacao'
    
    describe "#toEnv", ->
        return it "Reads to env", ->
            html_tmpl.readToEnv cacao.env

describe "Transition", ->
    
    describe "#fromFile", ->
        return it "Reads from file", ->
            code_tmpl = Cacao.Transition.fromFile '../examples/jinja_for.cacao'

    describe "#toEnv", ->
        return it "Reads to env", ->
            code_tmpl.equiv.template_refactoring.readToEnv cacao.env

    describe "#transform", ->
        return it "Transforms", ->
            cacao.env.jinja_text = code_tmpl.equiv.template_refactoring.tmpl_str
            code_tmpl.transform cacao.env.jinja_text, cacao.env

describe "Substitution", ->
    return it "substitutes", ->
        cacao.env.html_full = """<#{cacao.env.starttag} #{cacao.env.properties}>#body_jinja{}</#{cacao.env.starttag}>"""
        cacao.env.body_jinja = "#{cacao.env.hmtl_full}"
        code_tmpl.transform jinja_text, cacao.env
