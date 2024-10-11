
from enum import Enum
from .util import Range, strfind_delimiterInTmpl, strfind_untilDelimiter

class Stack(dict):
    stacktype = 'generic'
    config = dict()
    globals = dict()

    def __init__(self, stack_type: str, configuration: dict = dict()):
        super().__init__()
        self.stacktype = str(stack_type)
        self.config += configuration
        if configuration.globals is not None:
            self.globals += configuration.globals


class Environment(Stack):
    class Variable:
        class Type(Enum):
            store = 'store'
            read = 'read'
            create = 'create'

        name = ''
        value = ''
        type = ''
        tmpl_formatter = None

        def __init__(self, var_str):
            try:
                Environment.Variable.fromCode(var_str, self)
            except EnvironmentError as e:
                print(str(e))

        @staticmethod
        def fromCode(self, var_str: str, variable):
            is_there_a_formatter = var_str.rfind('}{')
            if len(var_str) < 3:
                raise EnvironmentError("'{0}' is not a var_str.".format(var_str))
            if is_there_a_formatter > -1:
                pos_formatter_start = 2 + var_str.find('}{')
                tmpl_str = var_str[pos_formatter_start:len(var_str)-2]  # Omit closing brace

            prefix_store = var_str.count('#', 0, 2)
            if prefix_store is 1:
                variable.type = Environment.Variable.Type.read
            elif prefix_store is 2:
                variable.type = Environment.Variable.Type.store
            elif prefix_store is 3:
                variable.type = Environment.Variable.Type.create
            else:
                raise EnvironmentError("'{0}' is not a var_str.".format(var_str))

            pos_var_name = 1 + var_str.rfind('#')
            if pos_var_name > 0:
                pos_var_value = 1 + var_str.find('{')
                if pos_var_value < 1:
                    raise EnvironmentError("'{0}' is not a var_str.".format(var_str))
                elif pos_var_value > 1:
                    variable.name = var_str[pos_var_name:pos_var_value-1]
                    pos_var_value_end = var_str.find('}', pos_var_value - 1)
                    if pos_var_value_end > -1:
                        variable.value = var_str[pos_var_value:pos_var_value_end]
                    else:
                        variable.value = str()
                else:
                    variable.name = var_str[pos_var_name:]

        @staticmethod
        def findVariable(var_str: str, start=0, end=0):
            if len(var_str) is 0:
                return None
            elif end is 0: # Set default depending parameters
                end = len(var_str) - 1
            pos_var_start = var_str.find('#', start, end)
            if pos_var_start > -1:
                has_tmpl = var_str.find('}{', pos_var_start, end)
                if has_tmpl > -1:
                    pos_var_end = 1 + var_str.find('}', has_tmpl+2, end)
                else:
                    pos_var_end = 1 + var_str.find('}', pos_var_start, end)
                if pos_var_end is 0:
                    raise EnvironmentError("There is no variable inside '{0}'.".format(var_str))
                return Range(pos_var_start, pos_var_end, var_str[pos_var_start:pos_var_end])
            return None

        @staticmethod
        def findVariables(var_str: str, start=0, end=0):
            if len(var_str) is 0:
                raise EnvironmentError("There is no variable inside '{0}'.".format(var_str))

            rangelist = [Range]
            pos_start_next_variable = start
            rng = Environment.Variable.findVariable(var_str, pos_start_next_variable, end)
            while rng is not None:
                rangelist += rng
                pos_start_next_variable = rng.end

            varenv = dict()
            for rng in rangelist:
                variable = Environment.Variable(rng.content)
                varenv[variable.name] = variable

            return [varenv, rangelist]


    class Template:
        name = ''
        tmpl_str = ''

        def __init__(self, namestr: str, tmplstr: str):
            self.name = namestr
            self.tmpl_str = tmplstr

        @staticmethod
        def fromCode(tmplstr: str):
            pos_tmpl_name = tmplstr.find('>>')
            if pos_tmpl_name > -1:
                pos_name_linebreak = tmplstr.find('\n', pos_tmpl_name)
                pos_tmpl_name += 2
                name = tmplstr[pos_tmpl_name:pos_name_linebreak-1]
                return Environment.Template(name, tmplstr[pos_name_linebreak+1:])
            else:
                return Environment.Template('', tmplstr)

        @staticmethod
        def fromFile(path: str):
            with open(path, 'r') as f:
                return Environment.Template.fromCode(f.read())

        def read(self):
            variable_list = [Environment.Variable]

            lane_list = self.tmpl_str.split('\n')
            for lane in lane_list:
                [varenv, rangelist] = Environment.Variable.findVariables(lane)

                for rng in rangelist:
                    variable = Environment.Variable(rng.content)
                    pos_tmpl_snippet = 2 + rng.content.find('}{')
                    if pos_tmpl_snippet > 1:
                        tmpl = Environment.Template.fromCode(rng.content[pos_tmpl_snippet:len(rng.content)-2])
                        variable.tmpl_formatter = tmpl
                    variable_list += variable

            return variable_list

        @staticmethod
        def findContentUntil(content_str: str, var_range_current: Range, var_range_next: Range):
            # Find the content between the two variables current and next
            pos_variable_end = 1 + var_range_current.end
            pos_next_variable = content_str.find('#', pos_variable_end + 1)
            content_between = content_str[pos_variable_end:pos_next_variable]

            # Find out their delimiter
            delim_range = strfind_delimiterInTmpl(content_str, var_range_current, var_range_next)

            # Find out with what the var_range_current is delimited
            # We're searching for the characters left and right of var_range_current and left of var_range_next
            if var_range_current.start <= 0:
                start = 0
            else:
                start = var_range_current.start - 1
            # We have content, that delimits the following content
            if len(var_range_current.content) > 0:
                delim_range_before = Range(var_range_current.end, var_range_current.end, var_range_current.content)
            elif var_range_current.start is 0:
                delim_range_before = Range(0, 0, '')
            elif content_str[start:var_range_current.start + 1].rfind('} ') > - 1: # We have a delimiter before in out template
                pos_var_end_before = content_str.rfind('} ', var_range_current.start -1)
                delim_between = content_str[pos_var_end_before+1:var_range_current.start-1]
                delim_range_before = Range(var_range_current.end, var_range_current.end, delim_between)
            else: # If there is a delimiting string before, use it. Else find out the template content of the variable
                pos_var_content_end_before = var_range_current.start - 1
                pos_var_end_before = content_str.rfind('} ', 0, var_range_current.start - 1)
                var_range_current_new = Range(pos_var_end_before, pos_var_content_end_before, content_between[pos_var_end_before:pos_var_content_end_before])

                delim_range_before = Environment.Template.findContentUntil(content_str, var_range_current_new, var_range_current)

            # Now the left and the right delimiter is set. Find its position in text
            pos_delim_left = content_str.find(delim_range_before.content, 0, content_str.rfind(delim_range.content))
            pos_delim_right = content_str.rfind(delim_range.content, 0, pos_delim_left)

            # Return everything from the left to the right delimiter
            return Range(pos_delim_left+1, pos_delim_right-1, content_str[pos_delim_left+1:pos_delim_right-1])

        @staticmethod
        def templateFormatter(stack: Stack, current_var, var_value: str):
            tmpl = current_var.tmpl_formatter
            return tmpl.transform(var_value, stack)

        def transformTo(self, stack: Stack, tmpl2):
            return self.transform(tmpl2.tmpl_str, stack)

        @staticmethod
        def findContentUntilDelimiter(content_str, delim=''):
            # If delim.length is 0, we're searching for all content until the first delimiter
            if len(delim) is 0: # All content, including content of a variable until the next variable counts
                pos_next_hashtag = content_str.find('#{')
                if pos_next_hashtag < 0:
                    return Range(0, len(content_str), content_str)
                vars = [varenv, rangelist] = Environment.Variable.findVariables(content_str, pos_next_hashtag)
                result = content_str[0:pos_next_hashtag-1]

                for idx, variable in enumerate(varenv):
                    result += variable.content
                    if idx + 1 < len(varenv):
                        result += Environment.Template.findContentUntil(content_str, rangelist[idx], rangelist[idx+1])
                    else:
                        result += str[rangelist[idx].end:]

                return Range(pos_next_hashtag, len(result)-1-pos_next_hashtag, result)
            elif content_str.find(delim) > -1:
                pos = content_str.find(delim)
                return Range(0, pos, content_str[:pos])
            else:
                return Range(0, 0, '')

        def transform(self, content_str: str, stack: Stack = Stack()):
            vars = [varenv, rangelist] = Environment.Variable.findVariables(self.tmpl_str)

            result = ''
            pos_in_str = 0
            for idx, rng in enumerate(rangelist):
                current_range = Environment.Template.findContentUntilDelimiter(content_str)
                variable = varenv[idx]

                # Store-value-Variables like ##start{<}
                if variable.type is Environment.Variable.Type.store:
                    stack[variable.name] = current_range.content

                    if variable.tmpl_formatter is not None:
                        result += '###' + variable.name + '{}' + self.templateFormatter(stack, variable, variable.value)
                    else:
                        result += '###' + variable.name + '{}'

                # Read-value-Variables like #starttag{}
                elif variable.type is Environment.Variable.Type.read:
                    if variable.tmpl_formatter is not None:
                        result += self.templateFormatter(stack, variable, stack[variable.name])
                    else:
                        result += stack[variable.name]

                # Variables like ###properties{}
                elif variable.type is Environment.Variable.Type.create:
                    if len(variable.value) > 0:
                        stack[variable.name] = variable.value

                    # Now everything else
                    if idx + 1 < len(rangelist):
                        content_range = self.findContentUntil(content_str, rng, rangelist[idx+1])
                    else:
                        content_range = content_str[rng.end+1:]

                    if variable.tmpl_formatter is not None:
                        result += self.templateFormatter(stack, variable, variable.value + content_range.content)

                else:
                    raise EnvironmentError("Could not transform string. Unknown variable type '#{{0}}'.".format(variable.type))

            return result

        def readToEnv(self, env):
            vars = self.read()
            for variable in vars:
                env.addvar(variable)

    def __init__(self, configuration: dict = dict()):
        super().__init__('format_env', configuration)
        self.variables = dict()
        self.templates = dict()

    def addvar(self, variable):
        self.variables[variable.name] = variable

    def addvar(self, name, value):
        self[name] = value
        self.variables[name] = Environment.Variable(value)

    def get(self, name):
        if self.variables[name] is None:
            return None
        return self.variables[name]

    def getValue(self, name):
        if self.variables[name] is None:
            return None
        return self.variables[name].value


