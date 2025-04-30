from celery import Celery
import requests

app = Celery('tasks', broker='redis://localhost:6379/0')

@app.task
def download_link(url):
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'
    }
    prefix = "https://www.consultant.ru"
    response = requests.get(prefix + url, headers=headers)
    
    if response.status_code == 200:
        with open(f"files/{url.replace("/", "-")}.html", "w") as file:
            file.write(response.text)
    else:
        print(url)
        print(f"Failed to retrieve the page. Status code: {response.status_code}")
