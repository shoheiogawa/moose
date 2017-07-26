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
import os
import multiprocessing
import shutil
import logging

import livereload

import MooseDocs
from MooseDocs.MooseMarkdown import MooseMarkdown
from MooseDocs import common

LOG = logging.getLogger(__name__)

def build_options(parser):
    """
    Command-line options for build command.
    """
    parser.add_argument('--config-file', type=str, default='website.yml',
                        help="The configuration file to use for building the documentation using "
                             "MOOSE. (Default: %(default)s)")
    parser.add_argument('--content', type=str, default='content.yml',
                        help="The YAML file containing the locations containing the markdown "
                             "files (Default: %(default)s). If the file doesn't exists the default "
                             "is {'default':{'base':'docs/content', 'include':'docs/content/*'}}")
    parser.add_argument('--dump', action='store_true',
                        help="Display the website file tree that will be created without "
                             "performing a build.")
    parser.add_argument('--clean', action='store_true',
                        help="Clean the 'site-dir', this happens by default when the '--serve' "
                             "command is used.")
    parser.add_argument('--num-threads', '-j', type=int, default=multiprocessing.cpu_count(),
                        help="Specify the number of threads to build pages with.")
    parser.add_argument('--template', type=str, default='website.html',
                        help="The template html file to utilize (Default: %(default)s).")
#    parser.add_argument('--include', type=str, default=None, nargs='+',
#                        help="Override the --content YAML file with the given directory patterns, "
#                             "this is mainly for increasing render times by limiting the pages to "
#                             "build. This option assumes the 'base' to be the local 'content'
#                             "directory.")

    parser.add_argument('--host', default='127.0.0.1', type=str,
                        help="The local host location for live web server (default: %(default)s).")
    parser.add_argument('--port', default='8000', type=str,
                        help="The local host port for live web server (default: %(default)s).")
    parser.add_argument('--site-dir', type=str, default=os.path.join(MooseDocs.ROOT_DIR, 'site'),
                        help="The location to build the website content (Default: %(default)s).")
    parser.add_argument('--serve', action='store_true',
                        help="Serve the presentation with live reloading, the 'site_dir' is "
                             "ignored for this case.")
    parser.add_argument('--no-livereload', action='store_true',
                        help="When --serve is used this flag disables the live reloading.")

class WebsiteBuilder(common.Builder):
    """
    Builder object for creating websites.
    """

    def __init__(self, content=None, **kwargs):
        super(WebsiteBuilder, self).__init__(**kwargs)
        if (content is None) or (not os.path.isfile(content)):
            LOG.info("Using default content directory configuration "
                     "(i.e., --content does not include a valid filename).")
            content = dict(default=dict(base=os.path.join(os.getcwd(), 'content'),
                                        include=[os.path.join(os.getcwd(), 'content', '*')]))
        else:
            content = MooseDocs.yaml_load(content)

        self._content = content

    def buildNodes(self):
        return common.moose_docs_file_tree(self._content)

class MooseDocsWatcher(livereload.watcher.Watcher):
    """
    A livereload watcher for MooseDocs that rebuilds the entire content if markdown files are
    added or removed.
    """
    def __init__(self, builder, num_threads, *args, **kwargs):
        super(MooseDocsWatcher, self).__init__(*args, **kwargs)
        self._builder = builder
        self._num_threads = num_threads
        self.init()

    def init(self):
        """
        Define the content to watch.
        """
        self._count = self._builder.count()
        for page in self._builder:
            self.watch(page.filename, lambda p=page: self._builder.buildPage(p), delay=2)

    def reset(self):
        """
        Perform a complete build and establish the items to watch.
        """

        # Clear the current tasks
        self._tasks = dict()
        self.filepath = None

        # Re-build
        LOG.info('START: Complete re-build of markdown content.')
        self._builder.build(num_threads=self._num_threads)
        self.init()
        LOG.info('FINISH: Complete re-build of markdown content.')

    def examine(self):
        """
        Override the default function to investigate if the number of markdown files changed.
        """
        self._builder.init()
        if self._count != self._builder.count():
            self.reset()
        else:
            super(MooseDocsWatcher, self).examine()
        return self.filepath, None

def build(config_file=None, site_dir=None, num_threads=None, no_livereload=False, content=None,
          dump=False, clean=False, serve=False, host=None, port=None, template=None,
          **template_args):
    """
    The main build command.
    """

    if serve:
        clean = True
        site_dir = os.path.abspath(os.path.join(MooseDocs.TEMP_DIR, 'site'))

    # Clean/create site directory
    if clean and os.path.exists(site_dir):
        LOG.info('Cleaning build directory: %s', site_dir)
        shutil.rmtree(site_dir)

    # Create the "temp" directory
    if not os.path.exists(site_dir):
        os.makedirs(site_dir)

    # Create the markdown parser
    config = MooseDocs.load_config(config_file, template=template, template_args=template_args)
    parser = MooseMarkdown(config)

    # Create the builder object and build the pages
    builder = WebsiteBuilder(parser=parser, site_dir=site_dir, content=content)
    builder.init()
    if dump:
        print builder
        return None
    builder.build(num_threads=num_threads)

    # Serve
    if serve:
        if not no_livereload:
            server = livereload.Server(watcher=MooseDocsWatcher(builder, num_threads))
        else:
            server = livereload.Server()
        server.serve(root=site_dir, host=host, port=port, restart_delay=0)

    return 0
