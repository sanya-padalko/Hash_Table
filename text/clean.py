import re

def clean_text(input_file, output_file):
    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()

    content = content.lower()
    clean_content = re.sub(r'[^\w\s]', '', content)
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(clean_content)

clean_text('orig_text2.txt', 'gamlet.txt')
