import pypandoc
from pypandoc.pandoc_download import download_pandoc
import glob
import os
import re


def convertMarkdownDocsToRst():
    # TODO Only download pandoc if it's not found
    download_pandoc()

    # TODO make various edits to improve conversion, like removing the Table of Contents
    input_dir = '../../doc'
    output_dir = 'generated_docs'
    subdirs = ['', 'how_tos', 'explanations']
    base_names_to_skip = ['README', 'TemplatePage']
    for subdir in subdirs:
        print(f'>>>> {subdir}')
        input_sub_dir = f'{input_dir}/{subdir}'
        if not os.path.isdir(input_sub_dir):
            print(f'Directory {input_sub_dir} does not exist. Skipping)')
        output_sub_dir = f'{output_dir}/{subdir}'
        md_files = glob.glob(f'{input_sub_dir}/*.md')
        if not md_files:
            continue
        if not os.path.isdir(output_sub_dir):
            os.makedirs(output_sub_dir)
        for file in md_files:
            file_base_file = os.path.split(file)[1]
            file_base_name = os.path.splitext(file_base_file)[0]

            if file_base_name in base_names_to_skip:
                continue
            print(file_base_name, input_sub_dir, output_sub_dir)
            convert_markdown_to_restructured_text(subdir, file_base_name, input_sub_dir, output_sub_dir)


def convert_markdown_to_restructured_text(subdir, file_base_name, input_dir, output_dir):
    with open(f'{input_dir}/{file_base_name}.md') as markdown_file:
        content = markdown_file.read()

        content = fix_up_markdown_content(subdir, file_base_name, content)
    output = pypandoc.convert_text(''.join(content), 'rst', format='md',
                                   outputfile=f'{output_dir}/{file_base_name}.rst')


def fix_up_markdown_content(subdir, file_base_name, content):
    # Remove 'GENERATED FILE' comment
    path_component = ''
    if len(subdir) > 0:
        path_component = subdir + '/'
    generated_file_message = \
        f"""<!--
GENERATED FILE - DO NOT EDIT
This file was generated by [MarkdownSnippets](https://github.com/SimonCropp/MarkdownSnippets).
Source File: /doc/{path_component}mdsource/{file_base_name}.source.md
To change this file edit the source file and then execute ./run_markdown_templates.sh.
-->
"""
    content = content.replace(generated_file_message, '')
    assert ('GENERATED FILE - DO NOT EDIT' not in content)

    # Remove table of contents
    content = re.sub(r'<!-- toc -->.*<!-- endtoc -->', '', content, count=1, flags=re.DOTALL)

    # Remove 'Back to User Guide'
    back_to_user_guide = (
        '---\n'
        '\n'
        '[Back to User Guide](/doc/README.md#top)\n'
    )
    content = content.replace(back_to_user_guide, '')

    # Remove 'snippet source' links from all code snippets
    # TODO Instead of master, use the changeset that this was generated from
    content = re.sub(
        r"<sup><a href='([^']+)' title='File snippet `[^`]+` was extracted from'>snippet source</a> ",
        r"(See [snippet source](https://github.com/approvals/ApprovalTests.cpp/blob/master\1))", content)

    # Remove 'anchor' links from all code snippets
    content = re.sub(
        r"\| <a href='#snippet-[^']+' title='Navigate to start of snippet `[^']+`'>anchor</a></sup>",
        '', content)

    # Fix "WARNING: Pygments lexer name 'h' is not known"
    # Todo: find out how to fix this in conf.py - this is a horrible hack!
    content = content.replace(
        '\n```h\n',
        '\n```cpp\n')

    # Fix "WARNING: Pygments lexer name 'txt' is not known"
    # Text files don't need any markup
    content = content.replace(
        '\n```txt\n',
        '\n```\n')

    # with open(file_base_name + '_hacked.md', 'w') as w:
    #     w.write(content)

    return content
