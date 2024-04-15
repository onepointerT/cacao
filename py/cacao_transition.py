
from .cacao_vte import Environment, Stack
from .util import genid

class Transition:
    class Equivalence:
        tmpl_before = None
        tmpl_refactoring = None

        def __init__(self, tmpl_before_str, tmpl_after_str):
            self.tmpl_before = Environment.Template.fromCode(tmpl_before_str)
            self.tmpl_refactoring = Environment.Template.fromCode(tmpl_after_str)

        def transformation(self, stack=Stack()):
            return self.tmpl_before.transformTo(stack, self.tmpl_refactoring)

    name = ''
    equiv = None

    def __init__(self, tmpl_before_str, tmpl_after_str, name=''):
        self.equiv = Transition.Equivalence(tmpl_before_str, tmpl_after_str)
        self.name = name

    def transform(self, content_str, stack=Stack()):
        transformation_tmpl = Environment.Template self.equiv.transformation(stack)
        return transformation_tmpl.transform(content_str, stack)

    @staticmethod
    def fromFile(path):
        try:
            with open(path, 'r') as f:
                fc = f.read()
                f.close()
        except FileNotFoundError:
            return None

        pos_equiv_1_start = fc.find('----\n')
        pos_equiv_2_start = fc.find('----\n', pos_equiv_1_start + 5)
        if pos_equiv_1_start is -1 or pos_equiv_2_start is -1:
            return None

        name = ''
        if pos_equiv_1_start is not 0:
            pos_tmpl_name = fc.find('>>', 0, pos_equiv_1_start)
            if pos_tmpl_name > -1:
                pos_name_linebreak = fc.find('\n', pos_tmpl_name, pos_equiv_1_start) + 2
                name = fc[pos_tmpl_name:pos_name_linebreak-1]
            else:
                name = genid()
        else:
            name = genid()

        return Transition(fc[pos_equiv_1_start+5:pos_equiv_2_start], fc[pos_equiv_2_start+5:], name)

