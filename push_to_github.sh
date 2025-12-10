#!/bin/bash

# Script d'aide pour pousser vers GitHub
# Usage: ./push_to_github.sh <URL_DU_REPO_GITHUB>

if [ -z "$1" ]; then
    echo " Erreur : Veuillez fournir l'URL de votre dépôt GitHub."
    echo "Usage : ./push_to_github.sh https://github.com/VOTRE_NOM/NOM_DU_REPO.git"
    exit 1
fi

REPO_URL=$1

echo " Configuration du dépôt distant..."
git remote add origin "$REPO_URL"

echo "Envoi des fichiers vers GitHub..."
git branch -M main
git push -u origin main

echo ""
echo " Terminé ! Votre projet est en ligne."
