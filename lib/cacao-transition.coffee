
import { Environment, Stack } from './cacao-vte'
import { File } from './path'

class Transition
    class Equivalence
        tmpl_before: undefined
        tmpl_refactoring: undefined

        constructor: (tmpl_before_str, tmpl_after_str) ->
            @tmpl_before = Environment.Template.fromCode tmpl_before
            @tmpl_refactoring = Environment.Template.fromCode tmpl_after
        
        transformation: (stack = new Stack()) ->
            return @tmpl_before.transformTo stack, @tmpl_refactoring
    

    name: ''
    equiv: undefined

    constructor: (tmpl_before_str, tmpl_after_str, name = '') ->
        equivalence = new Equivalence tmpl_before_str, tmpl_after_str
        @equiv = equivalence
        @name = name
    
    transform: (str, stack = new Stack()) ->
        transformation_tmpl = new Environment.Template equiv.transformation stack
        return transformation_tmpl.transform str, stack
    
    @fromFile: (path) ->
        fd = new File path
        if not fd.exists()
            return undefined
        fc = fd.read()

        pos_equiv_1_start = strfind fc, '----\n'
        pos_equiv_2_start = strfind fc, '----\n', pos_equiv_1_start + 5
        if pos_equiv_1_start is -1 or pos_equiv_2_start is -1
            return undefined

        name = ''
        if pos_equiv_1_start isnt 0
            pos_tmpl_name = strfind fc, '>>', 0, pos_equiv_1_start
            if pos_tmpl_name isnt -1
                pos_name_linebreak = strfind fc, '\n', pos_tmpl_name
                pos_tmpl_name += 2
            name = fc[pos_tmpl_name..pos_name_linebreak-1]
        else
            name = genid()
        
        return new Transition fc[pos_equiv_1_start+5..pos_equiv_2_start], fc[pos_equiv_2_start+5..], name
