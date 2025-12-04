Para o codigo principal dentro da pasta projetoiot_final tem os arquivos do codigo 'Codigo_final_projeto.cpp'. Nele está implementado o controle completo do locker, incluindo a lógica em que a porta deve trancar ou destrancar com a leitura de um determinado RFID já registrado no banco de dados. Além disso, foi implementado um sistema de segurança que verifica se a porta foi deixada aberta e não há movimento, indicando que o usuário foi embora sem fechar a porta. Nessa situação, um buzzer tocará, alertando que o locker foi deixado aberto. Há, também, o controle por meio de um servidor em que o adiministrador consegue, de forma remota, dar um override e abrir ou fechar determinado locker.

O controle de registro dos usuários no banco de dados é feito no Node-red (pelo arquivo .JSON mandado também nesse repositório) e pelo MQTT. Possibilitando, assim, a interação desses dois componentes
com o locker. Além disso, existe o controle de acessos enviado por esses dois elementos, onde é registrado o usuário, horario e o estado em que o locker se encontra (aberto ou fechado). 

Um dashboard no grafana consome os dados dos bancos para explicitar informacoes sobre a ocupacao dos lockers. 


