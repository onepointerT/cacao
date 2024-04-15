
from .cacao_vte import Environment as BaseEnvironment, Stack
from .cacao_transition import Transition as BaseTransition


class Cacao:
    class Environment(BaseEnvironment):
        pass

    class Variable(BaseEnvironment.Variable):
        pass

    class Template(BaseEnvironment.Template):
        pass

    class Transition(BaseTransition):
        pass

    env = Environment()
    tmpl = dict()
    transitions = dict()

    def __init__(self, configuration: dict = dict()):
        env_cfg = dict()
        if configuration['env'] is not None:
            env_cfg = configuration['env']
        self.env = BaseEnvironment(env_cfg)
        self.config = configuration

    def addtmpl(self, template: BaseEnvironment.Template):
        self.tmpl[template.name] = template

    def addtmpl(self, name, template_str: str):
        template = BaseEnvironment.Template(name, template_str)
        self.addtmpl(template)

    def addtransition(self, name, tmpl_str1: str, tmpl_str2: str):
        tran = BaseTransition(tmpl_str1, tmpl_str2, name)
        self.transitions[name] = tran

    def toString(self, content_str: str, stack: Stack):
        pos_next_variable = 0
        rest_str = content_str
        new_str = ''
        pos_next_variable = content_str.find('#', pos_next_variable)
        while pos_next_variable > -1:
            pos_end_next_variable = content_str.find('}', pos_next_variable)

            has_more_hashtags = True
            while has_more_hashtags:
                if content_str[pos_next_variable] != '#'
                    has_more_hashtags = False
                    break
                else:
                    pos_next_variable += 1
            varname = content_str[pos_next_variable:pos_end_next_variable-2]
            pos_next_variable = content_str.find('#', pos_end_next_variable)
            if pos_next_variable is -1:
                break
            rest_str = rest_str[pos_end_next_variable+1:pos_next_variable-1]
            new_str += stack[varname] + rest_str

        new_str += rest_str
        return new_str
