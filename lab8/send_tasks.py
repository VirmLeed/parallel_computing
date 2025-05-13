from tasks import download_link
import requests
from bs4 import BeautifulSoup
from tqdm import tqdm

def extract_links(url):
    response = requests.get(url)
    
    if response.status_code == 200:
        soup = BeautifulSoup(response.text, 'html.parser')
        
        links = [a['href'] for a in soup.find_all('a', href=True) if not a['href'].startswith(('http://', 'https://', '//www'))]
        
        return links
    else:
        print(f"Failed to retrieve the page. Status code: {response.status_code}")
        return []

url = 'https://www.consultant.ru/document/cons_doc_LAW_5142/'
links = extract_links(url)

for link in tqdm(links):
    result = download_link.delay(link)
    with open(f"files/{url.replace("/", "-")}.html", "w") as file:
      file.write(result.get(timeout = 10))
