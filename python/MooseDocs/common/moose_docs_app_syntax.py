#pylint: disable=missing-docstring
####################################################################################################
#                                    DO NOT MODIFY THIS HEADER                                     #
#                   MOOSE - Multiphysics Object Oriented Simulation Environment                    #
#                                                                                                  #
#                              (c) 2010 Battelle Energy Alliance, LLC                              #
#                                       ALL RIGHTS RESERVED                                        #
#                                                                                                  #
#                            Prepared by Battelle Energy Alliance, LLC                             #
#                               Under Contract No. DE-AC07-05ID14517                               #
#                               With the U. S. Department of Energy                                #
#                                                                                                  #
#                               See COPYRIGHT for full restrictions                                #
####################################################################################################
#pylint: enable=missing-docstring
import collections
import logging
import json

import mooseutils
from nodes import SyntaxNode, MooseObjectNode, ActionNode, MooseObjectActionNode

LOG = logging.getLogger(__name__)

def add_moose_object_helper(name, item, parent):
    """
    Helper to handle the Postprocessor/UserObject special case.

    This probably needs to be more generic, but this will do the job for now.
    """
    if ('moose_base' in item) and (item['moose_base'] == 'Postprocessor') and \
       (item['parent_syntax'] == 'UserObjects/*'):
        return
    MooseObjectNode(name, item, parent=parent)


def syntax_tree_helper(item, parent):
    """
    Tree builder helper function.

    This investigates the JSON nodes and builds the proper input file tree for MooseDocs.
    """

    if item is None:
        return

    if 'actions' in item:
        for key, action in item['actions'].iteritems():
            if ('parameters' in action) and action['parameters'] and \
            ('isObjectAction' in action['parameters']):
                MooseObjectActionNode(key, action, parent=parent)
            else:
                ActionNode(key, action, parent=parent)

    if 'star' in item:
        syntax_tree_helper(item['star'], parent)

    if ('types' in item) and item['types']:
        for key, obj in item['types'].iteritems():
            add_moose_object_helper(key, obj, parent)

    if ('subblocks' in item) and item['subblocks']:
        for k, v in item['subblocks'].iteritems():
            node = SyntaxNode(k, parent=parent)
            syntax_tree_helper(v, node)

    if ('subblock_types' in item) and item['subblock_types']:
        for k, v in item['subblock_types'].iteritems():
            add_moose_object_helper(k, v, parent)


def moose_docs_app_syntax(location, hide=None):
    """
    Creates a tree structure representing the MooseApp syntax for the given executable.

    Inputs:
        location[str]: The folder to locate Moose executable.
        hide[dict]: Items to consider "hidden".
    """

    exe = mooseutils.find_moose_executable(location)
    raw = mooseutils.runExe(exe, '--json')
    raw = raw.split('**START JSON DATA**\n')[1]
    raw = raw.split('**END JSON DATA**')[0]
    tree = json.loads(raw, object_pairs_hook=collections.OrderedDict)

    root = SyntaxNode('')
    for key, value in tree['blocks'].iteritems():
        node = SyntaxNode(key, parent=root)
        syntax_tree_helper(value, node)

    if hide is not None:
        for group, hidden in hide.iteritems():
            for h in hidden:
                filter_ = lambda n, hide=h, grp=group: (n.full_name.startswith(hide)) and \
                                    (grp == 'all' or grp in n.groups.keys())
                for node in root.findall(filter_=filter_):
                    node.hidden = True

    return root
