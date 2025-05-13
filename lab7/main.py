import requests
from bs4 import BeautifulSoup
from tqdm import tqdm
import threading
from multiprocessing import Process, Pool
from multiprocessing.dummy import Pool as thread_pool
import time

def extract_links(url):
    response = requests.get(url)
    
    if response.status_code == 200:
        soup = BeautifulSoup(response.text, 'html.parser')
        
        links = [a['href'] for a in soup.find_all('a', href=True) if not a['href'].startswith(('http://', 'https://', '//www'))]
        
        return links
    else:
        print(f"Failed to retrieve the page. Status code: {response.status_code}")
        return []

def download_link(url):
    headers = {
        'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64; rv:137.0) Gecko/20100101 Firefox/137.0'
    }
    prefix = "https://www.consultant.ru"
    try:
        response = requests.get(prefix + url, headers=headers)
    except requests.exceptions.ConnectionError:
        print("connection failed " + prefix + url, end="\r")
        return
    
    if response.status_code == 200:
        with open(f"files/{url.replace("/", "-")}.html", "w") as file:
            file.write(response.text)
    else:
        print(url)
        print(f"Failed to retrieve the page. Status code: {response.status_code}")



def serial_download(links):
    print("downloading links")
    for link in tqdm(links):
        download_link(link)

def threading_download(links):
    threads = []

    print("starting threads")
    for link in tqdm(links):
        thread = threading.Thread(target=download_link, args=(link,))
        threads.append(thread)
        thread.start()

    print("waiting for threads to finish")
    for thread in tqdm(threads):
        thread.join()

def multiprocessing_download(links):
    with Pool(processes=20) as pool:
        list(tqdm(pool.map(download_link, links)))

def threading_multiprocessing_download(links):
    with thread_pool(processes=20) as pool:
        pool.map(download_link, tqdm(links))

url = 'https://www.consultant.ru/document/cons_doc_LAW_5142/'
links = extract_links(url)[:80]

print("Starting threading download")
start = time.time()
threading_multiprocessing_download(links)
end = time.time()
took = end - start
print(f"Execution time: {took:.2f} seconds")

print("Starting multiprocessing download")
start = time.time()
multiprocessing_download(links)
end = time.time()
took = end - start
print(f"Execution time: {took:.2f} seconds")

print("Starting serial download")
start = time.time()
serial_download(links)
end = time.time()
took = end - start
print(f"Execution time: {took:.2f} seconds")
