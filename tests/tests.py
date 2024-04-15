
from .. import *

print("Template reads from file")
html_tmpl = Cacao.Environment.Template.fromFile('../example/html.cacao')
print(html_tmpl)

html_tmpl.readToEnv(cacao.env)
print("Template reads to env")

print("Transition reads from file")
code_tmpl = Cacao.Transition.fromFile('../example/jinja_for.cacao')
print(code_tmpl)

code_tmpl.equiv.tmpl_refactoring.readToEnv(cacao.env)
print("Transition reads to env")

code_tmpl.transform(cacao.env.jinja_text, cacao.env)
print("Transition transforms")

print("Substitution")
cacao.env.html_full = f"""<{cacao.env.starttag} {cacao.env.properties}>#body_jinja{}</{cacao.env.starttag}>"""
cacao.env.body_jinja = "{cacao.env.html_full}"
code_tmpl.transform(cacao.env.body_jinja, cacao.env)