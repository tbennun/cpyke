import ast
from typing import Any, Dict

__all__ = [
    'invoke',
    'pip_install'
]


class InputVariableFinder(ast.NodeVisitor):
    """ 
    Python AST visitor that finds ordered arguments for a given AST.
    """

    def __init__(self, defined=None):
        super(InputVariableFinder, self).__init__()
        self.defined = set(defined or {})
        self.args = []

    def visit_Call(self, node):
        # Skip visiting function names
        for arg in node.args:
            self.visit(arg)
        for kwarg in node.keywords:
            self.visit(kwarg)

    def visit_Attribute(self, node):
        # Skip visiting attributes
        self.visit(node.value)

    def visit_AnnAssign(self, node):
        # Skip visiting annotations
        self.visit(node.target)
        self.visit(node.value)

    def visit_Name(self, node):
        if isinstance(node.ctx, ast.Load) and node.id not in self.defined:
            if node.id.startswith('__cpyke'):
                raise NameError('Cannot define variables that start'
                                ' with "__cpyke"')
            self.args.append(node.id)
        self.defined.add(node.id)
        self.generic_visit(node)


def invoke(code, args, globs):
    # Parse Python code to AST
    parsed = ast.parse(code)

    # Find input variables
    variables = InputVariableFinder(globs)
    variables.visit(parsed)

    # Add return value to final expression
    if isinstance(parsed.body[-1], ast.Return):
        parsed.body[-1] = ast.copy_location(ast.Assign(
            targets=[ast.Name(id='__cpyke_return', ctx=ast.Store())],
            value=parsed.body[-1].value), parsed.body[-1])
    elif isinstance(parsed.body[-1], ast.Expr):
        parsed.body[-1] = ast.copy_location(ast.Assign(
            targets=[ast.Name(id='__cpyke_return', ctx=ast.Store())],
            value=parsed.body[-1].value), parsed.body[-1])
    parsed = ast.fix_missing_locations(parsed)

    # Execute modified code
    compiled = compile(parsed, '', 'exec')
    locs = {aname: arg for aname, arg in zip(variables.args, args)}
    exec(compiled, globs, locs)
    if '__cpyke_return' in locs:
        res = locs['__cpyke_return']
        del locs['__cpyke_return']
    else:
        res = None

    # Update new locals (except for the ones given externally)
    globs.update({k: v for k, v in locs.items() if k not in variables.args})
    
    return res

def pip_install(pkg):
    import os
    import subprocess
    import sys
    # Try to find interpreter
    interpreter = os.path.join(os.path.dirname(os.__file__), '..', 'bin', 'python')
    if not os.path.isfile(interpreter):
        interpreter += '.exe'
        if not os.path.isfile(interpreter):
            interpreter = os.path.join(os.path.dirname(os.__file__), '..', 'python')
            if not os.path.isfile(interpreter):
                interpreter += '.exe'
                if not os.path.isfile(interpreter):
                    raise LookupError('Cannot determine interpreter path')
    # Invoke the pip module
    subprocess.check_call([interpreter, '-m', 'pip', 'install', pkg])

if __name__ == '__main__':
    invoke('import numpy as np', [], globals())
    invoke('np.random.rand(n, m)', [20, 30], globals())
