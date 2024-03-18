# cacao
### Concurrent Arbitrary Consent Artificial Objects
#### v0.1.0

This small library is implemented to handle concurrent consent in arbitrary objects, operates on the alphabet with a simple syntax:

````cacao
* `###varname{}` stores the variable content from this position until the occurence of the next
  variable or a delimiter in between into the environments variable's stack
* `##varname{$value}` defines a value at this position
* `#varname{}` inserts the current value of varname from environment variables at this position
* A second brace pair behind a variable definition then transforms and formats with the script 
  and cacao environment variables, e.g. `##varname{#body{}}{#html_full{}}` replaces `#body{}`
  with the values from `cacao.env.html_full` on template substitution.
````

To use the library, please import the index file

````coffeescript
import 'cacao/index'
````

You can use the python file cacao

## Using

#### Setup

You might want to have a look at `examples/`. You might for example define your html template like this:

````js
html_tmpl = Cacao.Environment.Template.fromFile 'cacao/examples/html.cacao'
html_tmpl.readToEnv cacao.env
````

to setup the environment with one template only.

#### Example 1

To get the content inside both html tags in the variable `cacao.env.body_html`, you can work like this

````html
html_text = """<div class="container">
    Some text
    </div>"""
````
````js
html_tmpl.transform html_text, cacao.env
````

and you'll have the following variables in `cacao.env`:

````js
cacao.env.body_html == "Some text"
cacao.env.starttag == "div"
cacao.env.properties == """class="container""""
````

#### Example 2

If using a programming or scripting language, it might be useful to know operands, tags and variables and transform them. In this example we'll see, how more dynamic [Jinja](https://jinja.palletsprojects.com/en/3.0.x/templates/) could work.

Therefore there is a `Cacao.Transition` class, that can be used to make a fourier transformation or to determine variables
just from there position in a string.

````js
code_tmpl = Cacao.Transition.fromFile 'cacao/eamples/jinja_for.cacao'
code_tmpl.equiv.template_refactoring.readToEnv cacao.env
cacao.env.jinja_text = code_tmpl.template_refactoring.tmpl_str
````
````jinja
{% for elem in elements %}
    #body_jinja{}
{% endfor %}
````
````js
code_tmpl.transform jinja_text, cacao.env
````

should then give the following variables in the environment

````js
cacao.env.operand == "for"
cacao.env.what == "elem in elements"
cacao.env.varnames == "elem"
cacao.env.arrayname == "elements"
cacao.env.body_jinja == "#body_jinja{}"
````

As we can see, the next substitution would introduce everything from `cacao.env.body_jinja` variable into the templates's string, so we could to some magic for dynamic loops.

````js
cacao.env.html_full = """<#{cacao.env.starttag} #{cacao.env.properties}>#body_jinja{}</#{cacao.env.starttag}>"""
cacao.env.body_jinja = "#{cacao.env.hmtl_full}"
code_tmpl.transform jinja_text, cacao.env
````

would then give a html container per loop iteration in jinja and insert everything from jinja inside.

Mode and not -- more advanced examples and tests might follow.
