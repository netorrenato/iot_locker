Para o codigo principal dentro da pasta projetoiot_final tem os arquivos do codigo 'Codigo_final_projeto.cpp'. Nele está implementado o controle completo do locker, incluindo a lógica em que a porta deve trancar ou destrancar com a leitura de um determinado RFID já registrado no banco de dados. Além disso, foi implementado um sistema de segurança que verifica se a porta foi deixada aberta e não há movimento, indicando que o usuário foi embora sem fechar a porta. Nessa situação, um buzzer tocará, alertando que o locker foi deixado aberto. Há, também, o controle por meio de um servidor em que o adiministrador consegue, de forma remota, dar um override e abrir ou fechar determinado locker.

O controle de registro dos usuários no banco de dados é feito no Node-red (pelo arquivo .JSON mandado também nesse repositório) e pelo MQTT. Possibilitando, assim, a interação desses dois componentes
com o locker. Além disso, existe o controle de acessos enviado por esses dois elementos, onde é registrado o usuário, horario e o estado em que o locker se encontra (aberto ou fechado). 

Um dashboard no Grafana consome os dados dos bancos para explicitar informações sobre a ocupação dos lockers. Os gráficos mostrados ilustram o percentual de lockers ocupados a cada hora, quais lockers estão ocupados e o estado de cada locker ao longo do tempo. Foi também implementado um filtro, para que o usuário possa escolher quais lockers deseja incluir na visualização.

No banco de dados foram utilizadas e criados duas tabelas: a tabela **access** com o _ID do usuário_, _estado da porta_, _data e hora de seu acesso_ e _numero do locker_; e a tabela **users** com o _nome do usuario_, seu _ID_ e seu _RFID_.

Para a montagem do circuito, utilizamos o _EasyEDA_ para montar uma PCB compacta que se adequace as nossas especificações. Utilizando o _OnShape_, fizemos e imprimimos em 3D uma caixa para proteger a PCB e garantir uma maior organização na montagem do locker. 

(link para o OnShape: https://cad.onshape.com/documents/236972057961f44d27c5bdba/w/3e178e81a400f19d9918be7c/e/daddb86ca3d09b8bac02386e?renderMode=0&uiState=6930e76fbf49f2b960297c16)
